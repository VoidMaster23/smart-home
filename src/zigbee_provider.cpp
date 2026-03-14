
#include "zigbee_provider.h"
#include "device_factory.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

ZigbeeProvider::ZigbeeProvider(mqtt::async_client &client, QObject *parent)
    : DeviceProvider(parent), MqttMixin(client, "zigbee2mqtt/") {}

void ZigbeeProvider::handle_message(const QString &topic,
                                    const QJsonObject &payload) {
  if (!topic.startsWith(mqtt_topic_prefix()) || topic.endsWith("/set")) {
    return;
  }

  if (topic == mqtt_topic_prefix() + "bridge/devices") {
    QJsonArray devices = payload["devices"].toArray();
    process_discovery(devices);
    poll_all_devices();
    return;
  }

  QString name = topic.mid(mqtt_topic_prefix().length());

  if (name.contains("bridge")) {
    return;
  }
  route_update(name, payload);
}

void ZigbeeProvider::process_discovery(const QJsonArray &devices) {
  for (const auto data : devices) {
    auto load = data.toObject();
    if (load["type"] == "Coordinator") {
      continue;
    }

    auto id = load["ieee_address"].toString();

    if (id.isEmpty() || this->m_devices.contains(id)) {
      continue;
    }

    auto device = DeviceFactory::create_device(load);

    if (device != nullptr) {
      m_devices[id] = device;
      m_name_to_id[device->name()] = device->id();

      connect(device, &SmartDevice::request_command, this,
              [this, device](const QJsonObject &command) {
                QString topic = mqtt_topic_prefix() + device->name() + "/set";
                QJsonDocument doc(command);
                mqtt::message_ptr pubmsg = mqtt::make_message(
                    topic.toStdString(),
                    doc.toJson(QJsonDocument::Compact).toStdString());
                mqtt_client()->publish(pubmsg);
              });

      emit device_discovered(device);

      QString id = device->id();
      QString name = device->name();

      connect(device, &QObject::destroyed, this, [this, id, name] {
        m_devices.remove(id);
        m_name_to_id.remove(name);
        emit device_removed(id);
      });
    }
  }
}

void ZigbeeProvider::route_update(const QString &friendly_name,
                                  const QJsonObject &payload) {
  if (!m_name_to_id.contains(friendly_name)) {
    return;
  }
  auto id = m_name_to_id.value(friendly_name);
  auto device = m_devices.value(id);

  if (device != nullptr) {
    device->handle_update(payload);
  };
}

void ZigbeeProvider::on_connected() {
  QString topic = mqtt_topic_prefix() + "bridge/devices";
  mqtt_client()->publish(mqtt::make_message(topic.toStdString(), "", 1, false));
}

void ZigbeeProvider::poll_all_devices() {
  for (const auto &device : m_devices) {
    if(device == nullptr) {
        continue;
    }
    QString topic = mqtt_topic_prefix() + device->name() + "/get";
    QString payload = R"({"state": ""})";
    mqtt::message_ptr pubmsg =
        mqtt::make_message(topic.toStdString(), payload.toStdString());

    mqtt_client()->publish(pubmsg);
  }
}
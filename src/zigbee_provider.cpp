#include "zigbee_provider.h"
#include "device_factory.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>
#include <QSet>

/**
 * @brief Create a Zigbee provider bound to an MQTT client and the
 * "zigbee2mqtt/" topic prefix.
 */
ZigbeeProvider::ZigbeeProvider(mqtt::async_client &client, QObject *parent)
    : DeviceProvider(parent), MqttMixin(client, "zigbee2mqtt/") {}

/**
 * @brief Process an incoming MQTT message and route it to the appropriate
 * handler.
 */
void ZigbeeProvider::handle_message(
    const QString &topic, // NOLINT(bugprone-easily-swappable-parameters)
    const QByteArray &payload) {
  if (!topic.startsWith(mqtt_topic_prefix())) {
    return;
  }

  const QString rel_topic = topic.mid(mqtt_topic_prefix().length());

  if (rel_topic.endsWith("/set")) {
    return;
  }

  const QJsonDocument doc = QJsonDocument::fromJson(payload);
  if (doc.isNull()) {
    return;
  }

  if (rel_topic == "bridge/devices") {
    QJsonArray devices;
    if (doc.isArray()) {
      devices = doc.array();
    } else if (doc.isObject()) {
      devices = doc.object().value("devices").toArray();
    }

    process_discovery(devices);
    if (!devices.isEmpty()) {
      poll_all_devices();
    }
    return;
  }

  if (rel_topic == "bridge" || rel_topic.startsWith("bridge/")) {
    return;
  }

  if (doc.isObject()) {
    route_update(rel_topic, doc.object());
  }
}

/**
 * @brief Refactored discovery process to reduce cognitive complexity.
 */
void ZigbeeProvider::process_discovery(const QJsonArray &devices) {
  const QSet<QString> snapshot_ids = extract_snapshot_ids(devices);
  register_new_devices(devices);
  reconcile_missing_devices(snapshot_ids);
}

/**
 * @brief Extracts all valid IEEE addresses from the device list.
 */
QSet<QString> ZigbeeProvider::extract_snapshot_ids(const QJsonArray &devices) {
  QSet<QString> ids;
  for (const auto &data : devices) {
    const auto load = data.toObject();
    if (load.value("type") == "Coordinator") {
      continue;
    }
    const auto id = load.value("ieee_address").toString();
    if (!id.isEmpty()) {
      ids.insert(id);
    }
  }
  return ids;
}

/**
 * @brief Registers any device from the list that isn't already known.
 */
void ZigbeeProvider::register_new_devices(const QJsonArray &devices) {
  for (const auto &data : devices) {
    const auto load = data.toObject();
    if (load.value("type") == "Coordinator") {
      continue;
    }

    const auto id = load.value("ieee_address").toString();
    if (id.isEmpty() || m_devices.contains(id)) {
      continue;
    }

    const QPointer<SmartDevice> device = DeviceFactory::create_device(load);
    if (device != nullptr) {
      setup_new_device(device);
    }
  }
}

/**
 * @brief Configures a newly created device and connects its signals.
 */
void ZigbeeProvider::setup_new_device(const QPointer<SmartDevice> &device) {
  if (device == nullptr) {
    return;
  }

  const QString id = device->id();
  const QString name = device->name();

  m_devices[id] = device;
  m_name_to_id[name] = id;

  connect(device.get(), &SmartDevice::request_command, this,
          [this, device](QJsonObject command) {
            if (command.contains("state") && command["state"].isBool()) {
              command["state"] = command["state"].toBool() ? "ON" : "OFF";
            }
            const QString topic = mqtt_topic_prefix() + device->name() + "/set";
            const QJsonDocument doc(command);
            const mqtt::message_ptr pubmsg = mqtt::make_message(
                topic.toStdString(),
                doc.toJson(QJsonDocument::Compact).toStdString());
            mqtt_client()->publish(pubmsg);
          });

  emit device_discovered(device);

  connect(device.get(), &QObject::destroyed, this, [this, id, name] {
    if (m_devices.contains(id)) {
      m_devices.remove(id);
      m_name_to_id.remove(name);
      emit device_removed(id);
    }
  });
}

/**
 * @brief Removes devices that are no longer present in the snapshot.
 */
void ZigbeeProvider::reconcile_missing_devices(
    const QSet<QString> &snapshot_ids) {
  auto it = m_devices.begin();
  while (it != m_devices.end()) {
    const QString id = it.key();
    if (!snapshot_ids.contains(id)) {
      const QPointer<SmartDevice> device = it.value();
      if (device != nullptr) {
        m_name_to_id.remove(device->name());
        device->deleteLater();
      }
      it = m_devices.erase(it);
      emit device_removed(id);
    } else {
      ++it;
    }
  }
}

/**
 * @brief Routes an incoming payload to the device identified by its friendly
 * name.
 */
void ZigbeeProvider::route_update(
    const QString
        &friendly_name, // NOLINT(bugprone-easily-swappable-parameters)
    const QJsonObject &payload) {
  if (!m_name_to_id.contains(friendly_name)) {
    return;
  }
  const QString id = m_name_to_id.value(friendly_name);
  const QPointer<SmartDevice> device = m_devices.value(id);

  if (device != nullptr) {
    device->handle_update(payload);
  }
}

/**
 * @brief Requests Zigbee2MQTT to enumerate devices.
 */
void ZigbeeProvider::on_connected() {
  const QString topic = mqtt_topic_prefix() + "bridge/devices";
  mqtt_client()->publish(mqtt::make_message(topic.toStdString(), "", 1, false));
}

/**
 * @brief Requests state updates from every known Zigbee device.
 */
void ZigbeeProvider::poll_all_devices() {
  for (const auto &device : m_devices) {
    if (device == nullptr) {
      continue;
    }
    const QString topic = mqtt_topic_prefix() + device->name() + "/get";
    const QString payload = R"({"state": ""})";
    const mqtt::message_ptr pubmsg =
        mqtt::make_message(topic.toStdString(), payload.toStdString());

    mqtt_client()->publish(pubmsg);
  }
}

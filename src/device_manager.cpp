#include "device_manager.h"
#include "device_factory.h"
#include "smart_device.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>
#include <cstdlib>
#include <iostream>
#include <mqtt/async_client.h>
#include <string>

const std::string mqtt_address{std::string(std::getenv("MQTT_ADDRESS"))};

DeviceManager::DeviceManager(QObject *parent)
    : QObject(parent), m_client(mqtt_address) {
  try {
    m_client.set_callback(*this);
  } catch (...) {
    std::cout << "Could not connect to the bih" << '\n';
  }
}

QList<QPointer<SmartDevice>> DeviceManager::devices() const {
  return m_devices.values();
}

QPointer<SmartDevice> DeviceManager::get_device(QString &id) const {
  return m_devices.value(id, nullptr);
}

void DeviceManager::handle_message(QString &topic, QJsonObject &payload) {
  std::cout << "Topic: " << topic.toStdString() << '\n';

  if (topic.endsWith("/set")) {
    return;
  }

  for (const auto& device : m_devices) {
    QString device_topic = "zigbee2mqtt/" + device->friendly_name();

    if (topic == device_topic) {
      device->handle_update(payload);
      return;
    }
  }

  QJsonDocument data{payload};

  std::cout << "Payload: " << data.toJson(QJsonDocument::Indented).toStdString()
            << '\n';
}

void DeviceManager::add_new_device(QJsonArray const &payload) {
  for (const auto data : payload) {
    auto load = data.toObject();
    if (load["type"] == "Coordinator") {
      continue;
    }

    auto ieee_address = load["ieee_address"].toString();

    if (ieee_address.isEmpty() || this->m_devices.contains(ieee_address)) {
      continue;
    }

    auto device = DeviceFactory::create_device(load);
    // it is  a device we support
    if (device != nullptr) {
      device->setParent(this);
      this->m_devices[ieee_address] = device;

      emit devices_changed();
      emit device_discovered(device);
      std::cout << "Device " << device->friendly_name().toStdString()
                << " successfuully added" << "\n";
      connect(device, &SmartDevice::send_command, this,
              [this](const QString &topic, const QString &payload) {
                mqtt::message_ptr pubmsg = mqtt::make_message(
                    topic.toStdString(), payload.toStdString());
                m_client.publish(pubmsg);
              });
    }
  }

  for (const auto& device : m_devices) {
    QString topic = "zigbee2mqtt/" + device->friendly_name() + "/get";
    QString payload = R"({"state": ""})";
    mqtt::message_ptr pubmsg =
        mqtt::make_message(topic.toStdString(), payload.toStdString());

    m_client.publish(pubmsg);
  }
}

void DeviceManager::message_arrived(mqtt::const_message_ptr msg) {
  QString topic{QString::fromStdString(msg->get_topic())};
  QByteArray payload{QByteArray::fromStdString(msg->get_payload_str())};

  // for the most part I expect that everything will be primarily single JSON
  // objects for now this is crude but it works
  // TODO: improve this by making it so that we have a nice little detector for
  // array vs json values.
  if (topic.contains("zigbee2mqtt/bridge") && !topic.contains("devices")) {
    return;
  }

  if (topic.contains("bridge/devices")) {
    QJsonArray payload_json_arr = QJsonDocument::fromJson(payload).array();
    QMetaObject::invokeMethod(
        this, [this, payload_json_arr]() { add_new_device(payload_json_arr); });
    return;
  }

  QJsonObject payload_json{QJsonDocument::fromJson(payload).object()};

  DeviceManager::handle_message(topic, payload_json);
};

void DeviceManager::connect_to_broker() {
  try {
    mqtt::connect_options opts = mqtt::connect_options_builder()
                                     .clean_session()
                                     .automatic_reconnect()
                                     .finalize();

    m_client.connect(opts)->wait();
    m_client.subscribe("zigbee2mqtt/#", 1);
    std::cout << "Requesting device list..." << '\n';
    m_client.publish("zigbee2mqtt/bridge/devices", "", 1, false)->wait();

    std::cout << "Connected and Subbed " << "\n";
  } catch (const mqtt::exception &exc) {
    std::cerr << exc.get_error_str() << "\n";
  }
}

void DeviceManager::connection_lost(const std::string & /*cause*/) {};
void DeviceManager::delivery_complete(mqtt::delivery_token_ptr /*token*/){};

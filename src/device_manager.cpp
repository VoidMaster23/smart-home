#include "device_manager.h"
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

QList<QObject *> DeviceManager::devices() const {
  QList<QObject *> devices;

  for (qsizetype i = 0; i < this->m_devices.size(); ++i) {
    devices.push_back(m_devices.at(i));
  }

  return devices;
}

SmartDevice *DeviceManager::get_device(QString &id) const {
  for (auto *m_device : m_devices) {
    auto *device{m_device};
    if (device->device_id().compare(id, Qt::CaseInsensitive) == 0) {
      return device;
    }
  }
  return nullptr;
}

void DeviceManager::handle_message(QString &topic, QJsonObject &payload) {
  std::cout << "Topic: " << topic.toStdString() << '\n';

  QJsonDocument data{payload};

  std::cout << "Payload: " << data.toJson(QJsonDocument::Indented).toStdString()
            << '\n';
}

void DeviceManager::message_arrived(mqtt::const_message_ptr msg) {
  QString topic{QString::fromStdString(msg->get_topic())};
  QByteArray payload{QByteArray::fromStdString(msg->get_payload_str())};

  // for the most part I expect that everything will be primarily single JSON objects
  // for now this is crude but it works
  // TODO: improve this by making it so that we have a nice little detector for array vs json values. 
  
  if (topic.contains("zigbee2mqtt/bridge") && !topic.contains("devices")) {
    return;
  }

  if (topic.contains("bridge/devices")) {
    std::cout << payload.toStdString() << "\n";
    QJsonArray payload_json_arr = QJsonDocument::fromJson(payload).array();
    std::cout << payload_json_arr.size();
    for (auto data : payload_json_arr) {
      auto load = data.toObject();
      if (load["friendly_name"] != "Coordinator") {
        DeviceManager::handle_message(topic, load);
      }
    }
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

void DeviceManager::connection_lost(const std::string &cause) {};
void DeviceManager::delivery_complete(mqtt::delivery_token_ptr token) {};

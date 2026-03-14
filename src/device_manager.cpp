#include "device_manager.h"
#include "zigbee_provider.h"
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
    
    // Initialize providers
    auto* zigbee = new ZigbeeProvider(m_client, this); //NOLINT
    m_providers.append(zigbee);

    connect(zigbee, &DeviceProvider::device_discovered, this, &DeviceManager::on_device_discovered);
    connect(zigbee, &DeviceProvider::device_removed, this, &DeviceManager::on_device_removed);

  } catch (...) {
    std::cerr << "Could not connect to the MQTT broker" << '\n';
  }
}

QList<QPointer<SmartDevice>> DeviceManager::devices() const {
  return m_devices.values();
}

QPointer<SmartDevice> DeviceManager::get_device(const QString &id) const {
  return m_devices.value(id, nullptr);
}

void DeviceManager::on_device_discovered(QPointer<SmartDevice> device) {
  if (!device) return;
  
  m_devices[device->id()] = device;
  emit devices_changed();
  emit device_discovered(device.get());
  
  std::cout << "Device " << device->name().toStdString()
            << " successfully discovered and added to manager" << "\n";
}

void DeviceManager::on_device_removed(const QString &id) {
  if (m_devices.remove(id) > 0) {
    emit devices_changed();
    std::cout << "Device " << id.toStdString() << " removed from manager" << "\n";
  }
}

void DeviceManager::message_arrived(mqtt::const_message_ptr msg) {
  QString topic = QString::fromStdString(msg->get_topic());
  QByteArray payload_bytes = QByteArray::fromStdString(msg->get_payload_str());
  
  // We expect JSON payloads for all our devices/providers
  QJsonDocument doc = QJsonDocument::fromJson(payload_bytes);
  QJsonObject payload;
  
  if (doc.isObject()) {
    payload = doc.object();
  } else if (doc.isArray()) {
    // Some messages (like bridge/devices) might be arrays, wrap them or let providers handle
    payload["devices"] = doc.array();
  }

  // Broadcast to all providers
  for (auto& provider : m_providers) {
    if (provider) {
        provider->handle_message(topic, payload);
    }
  }
}

void DeviceManager::connect_to_broker() {
  try {
    mqtt::connect_options opts = mqtt::connect_options_builder()
                                     .clean_session()
                                     .automatic_reconnect()
                                     .finalize();

    m_client.connect(opts)->wait();
    
    // Subscribe to all topics so providers can filter what they need
    m_client.subscribe("#", 1);
    
    std::cout << "Connected and Subscribed to all topics" << "\n";

    // Notify all providers that we are connected so they can perform discovery/polling
    for (auto& provider : m_providers) {
        if (provider) {
            provider->on_connected();
        }
    }

  } catch (const mqtt::exception &exc) {
    std::cerr << "MQTT Exception: " << exc.get_error_str() << "\n";
  }
}

void DeviceManager::connection_lost(const std::string & /*cause*/) {};
void DeviceManager::delivery_complete(mqtt::delivery_token_ptr /*token*/){};

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

/**
 * @brief Constructs a DeviceManager and initializes MQTT integration and device providers.
 *
 * Initializes the MQTT client callback, creates and registers the Zigbee provider, and connects
 * provider signals for device discovery and removal to the manager's handlers. Any exception
 * thrown during initialization is caught and reported to standard error.
 *
 * @param parent QObject ownership parent; ownership of created providers is transferred to this QObject.
 */
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

/**
 * Retrieve the managed device matching the given identifier.
 *
 * @param id Identifier of the device to look up.
 * @return QPointer<SmartDevice> Pointer to the matching device, or `nullptr` if no device with that id is managed.
 */
QPointer<SmartDevice> DeviceManager::get_device(const QString &id) const {
  return m_devices.value(id, nullptr);
}

/**
 * @brief Register a newly discovered device with the manager and notify listeners.
 *
 * Adds the provided device to the internal device map keyed by the device's id,
 * emits devices_changed() and device_discovered() to notify observers, and logs the discovery.
 *
 * @param device QPointer to the discovered SmartDevice; if null, the call is ignored.
 */
void DeviceManager::on_device_discovered(QPointer<SmartDevice> device) {
  if (!device) return;
  
  m_devices[device->id()] = device;
  emit devices_changed();
  emit device_discovered(device.get());
  
  std::cout << "Device " << device->name().toStdString()
            << " successfully discovered and added to manager" << "\n";
}

/**
 * @brief Remove a device with the given identifier from the manager.
 *
 * If a device with the specified id is present, it is removed, the
 * devices_changed() signal is emitted, and a removal message is logged.
 *
 * @param id Identifier of the device to remove; no action is taken if no device matches.
 */
void DeviceManager::on_device_removed(const QString &id) {
  if (m_devices.remove(id) > 0) {
    emit devices_changed();
    std::cout << "Device " << id.toStdString() << " removed from manager" << "\n";
  }
}

/**
 * @brief Parse an incoming MQTT message payload as JSON and forward it with the topic to all providers.
 *
 * If the payload is a JSON object it is forwarded unchanged; if the payload is a JSON array it is wrapped
 * into an object under the "devices" key before forwarding. Each non-null provider receives the topic and
 * resulting JSON object via its handle_message method.
 *
 * @param msg Pointer to the received MQTT message.
 */
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

/**
 * @brief Establishes connection to the configured MQTT broker and initializes providers.
 *
 * Connects the MQTT client using a clean session with automatic reconnect, subscribes to all
 * topics so providers may filter messages themselves, and invokes on_connected() on each
 * registered provider after a successful connection.
 *
 * Any mqtt::exception encountered during connect/subscribe is caught and written to stderr.
 */
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

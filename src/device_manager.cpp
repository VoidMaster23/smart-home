#include "device_manager.h"
#include "smart_device.h"

#include <QDebug>
#include <QList>
#include <QObject>
#include <QString>
#include <cstdlib>
#include <iostream>
#include <mqtt/async_client.h>
#include <stdexcept>
#include <string>

static std::string get_mqtt_address() {
  const char *address = std::getenv("MQTT_ADDRESS");
  if (address == nullptr) {
    throw std::runtime_error("MQTT address has not been set. Please set this "
                             "as an environment variable and try again");
  }

  return address;
};

/**
 * @brief Constructs a DeviceManager and initializes MQTT integration.
 *
 * Initializes the MQTT client callback.
 *
 * @param parent QObject ownership parent.
 */
DeviceManager::DeviceManager(QObject *parent)
    : QObject(parent), m_client(get_mqtt_address(), "client") {
  try {
    m_client.set_callback(*this);
  } catch (...) {
    std::cerr << "Could not connect to the MQTT broker" << '\n';
  }
}

/**
 * @brief Register a device provider with the manager.
 *
 * @param provider Pointer to the DeviceProvider to register.
 */
void DeviceManager::register_provider(DeviceProvider *provider) {
  if (provider == nullptr) {
    return;
  }

  m_providers.append(provider);
  connect(provider, &DeviceProvider::device_discovered, this,
          &DeviceManager::on_device_discovered);
  connect(provider, &DeviceProvider::device_removed, this,
          &DeviceManager::on_device_removed);
}

QList<QPointer<SmartDevice>> DeviceManager::devices() const {
  return m_devices.values();
}

/**
 * Retrieve the managed device matching the given identifier.
 *
 * @param id Identifier of the device to look up.
 * @return QPointer<SmartDevice> Pointer to the matching device, or `nullptr` if
 * no device with that id is managed.
 */
QPointer<SmartDevice> DeviceManager::get_device(const QString &id) const {
  return m_devices.value(id, nullptr);
}

/**
 * @brief Register a newly discovered device with the manager and notify
 * listeners.
 *
 * Adds the provided device to the internal device map keyed by the device's id,
 * emits devices_changed(). If the device is new to the manager, it also emits
 * device_discovered() and logs the discovery.
 *
 * @param device QPointer to the discovered SmartDevice; if null, the call is
 * ignored.
 */
void DeviceManager::on_device_discovered(const QPointer<SmartDevice> &device) {
  if (device == nullptr) {
    return;
  }

  const QString id = device->id();
  const bool is_new = !m_devices.contains(id);

  m_devices[id] = device;
  emit devices_changed();

  if (is_new) {
    emit device_discovered(device.get());
    std::cout << "Device " << device->name().toStdString()
              << " successfully discovered and added to manager" << "\n";
  } else {
    std::cout << "Device " << device->name().toStdString()
              << " (ID: " << id.toStdString() << ") updated in manager" << "\n";
  }
}

/**
 * @brief Remove a device with the given identifier from the manager.
 *
 * If a device with the specified id is present, it is removed, the
 * devices_changed() signal is emitted, and a removal message is logged.
 *
 * @param id Identifier of the device to remove; no action is taken if no device
 * matches.
 */
void DeviceManager::on_device_removed(const QString &id) {
  if (m_devices.remove(id) > 0) {
    emit devices_changed();
    std::cout << "Device " << id.toStdString() << " removed from manager"
              << "\n";
  }
}

/**
 * @brief Forward an incoming MQTT message payload as a raw byte array to all
 * registered providers.
 *
 * Each non-null provider receives the topic and the original payload bytes via
 * its handle_message method.
 *
 * @param msg Pointer to the received MQTT message.
 */
void DeviceManager::message_arrived(mqtt::const_message_ptr msg) {
  const QString topic = QString::fromStdString(msg->get_topic());
  const QByteArray payload = QByteArray::fromStdString(msg->get_payload_str());

  // Broadcast raw bytes to all providers; providers own parsing (JSON, etc.)
  for (auto &provider : m_providers) {
    if (provider != nullptr) {
      provider->handle_message(topic, payload);
    }
  }
}

/**
 * @brief Establishes connection to the configured MQTT broker and initializes
 * providers.
 *
 * Connects the MQTT client using a clean session with automatic reconnect,
 * subscribes to all topics so providers may filter messages themselves, and
 * invokes on_connected() on each registered provider after a successful
 * connection.
 *
 * Any mqtt::exception encountered during connect/subscribe is caught and
 * written to stderr.
 */
void DeviceManager::connect_to_broker() {
  try {
    const mqtt::connect_options opts = mqtt::connect_options_builder()
                                           .clean_session()
                                           .automatic_reconnect()
                                           .finalize();

    m_client.connect(opts)->wait();

  } catch (const mqtt::exception &exc) {
    qDebug() << "[ERROR] failed to establish MQTT connection"
             << QString::fromStdString(exc.get_error_str());
  }
}

void DeviceManager::notify_providers_connected() {
  // Notify all providers that we are connected so they can perform
  // discovery/polling
  for (auto &provider : m_providers) {
    if (provider != nullptr) {
      const QPointer<DeviceProvider> &thread_safe_provider = provider;
      QMetaObject::invokeMethod(
          provider,
          [thread_safe_provider]() {
            if (thread_safe_provider != nullptr) {
              thread_safe_provider->on_connected();
            }
          },
          Qt::QueuedConnection);
    }
  }
}

void DeviceManager::connected(const std::string & /*cause*/) {
  try {
    m_client.subscribe("#", 1);

    qDebug() << "[LOG] Connected and subscribed to all topics";

    notify_providers_connected();

  } catch (const mqtt::exception &exc) {
    qDebug() << "[ERROR] failed to subscribe to all topics"
             << QString::fromStdString(exc.get_error_str());
  }
}

void DeviceManager::connection_lost(const std::string &cause) {
  qDebug() << "[LOG] connection lost: " << QString::fromStdString(cause);
}
void DeviceManager::delivery_complete(mqtt::delivery_token_ptr /*token*/) {}

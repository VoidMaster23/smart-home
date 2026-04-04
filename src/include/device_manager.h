#pragma once

#include <QHash>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QStringView>
#include <mqtt/async_client.h>

#include "device_provider.h"
#include "smart_device.h"

/**
 * @brief Manages a collection of smart devices and their providers.
 *
 * DeviceManager acts as a central hub for discovering, tracking, and
 * communicating with smart devices via various providers. It also handles MQTT
 * connection.
 */
class DeviceManager : public QObject, public virtual mqtt::callback {
  Q_OBJECT
  Q_PROPERTY(
      QList<QPointer<SmartDevice>> devices READ devices NOTIFY devices_changed)

public:
  /**
   * @brief Construct a DeviceManager.
   * @param parent Optional QObject parent.
   */
  explicit DeviceManager(QObject *parent = nullptr);

  /**
   * @brief Register a device provider.
   * @param provider Pointer to the DeviceProvider to register.
   */
  void register_provider(DeviceProvider *provider);

  /**
   * @brief Get the internal MQTT client.
   * @returns Reference to the mqtt::async_client.
   */
  mqtt::async_client &mqtt_client() { return m_client; }

  /**
   * @brief Get the list of managed devices.
   * @returns QList of pointers to managed SmartDevice instances.
   */
  [[nodiscard]] QList<QPointer<SmartDevice>> devices() const;

  /**
   * @brief Get a device by its unique identifier.
   * @param id Unique identifier of the device.
   * @returns QPointer to the SmartDevice if found, or null otherwise.
   */
  [[nodiscard]] QPointer<SmartDevice> get_device(QStringView id) const;

  /**
   * @brief Called when MQTT connection is lost.
   * @param cause Reason for connection loss.
   */
  void connection_lost(const std::string &cause) override;

  /**
   * @brief Called when an MQTT message arrives.
   * @param msg The received MQTT message.
   */
  void message_arrived(mqtt::const_message_ptr msg) override;

  /**
   * @brief Called when MQTT delivery is complete.
   * @param token Token for the delivered message.
   */
  void delivery_complete(mqtt::delivery_token_ptr token) override;

  /**
   * @brief Establish connection to the MQTT broker.
   */
  void connect_to_broker();

  /**
   * @brief  called when the MQTT connection is established
   * @param cause Reason for connection
   */
  void connected(const std::string &cause) override;

signals:
  /**
   * @brief Emitted when the list of managed devices changes.
   */
  void devices_changed();

  /**
   * @brief Emitted when a new device is discovered.
   * @param device Pointer to the newly discovered SmartDevice.
   */
  void device_discovered(SmartDevice *device);

private slots:
  /**
   * @brief Internal slot to handle device discovery from providers.
   * @param device Pointer to the discovered device.
   */
  void on_device_discovered(const QPointer<SmartDevice> &device);

  /**
   * @brief Internal slot to handle device removal from providers.
   * @param id Unique identifier of the removed device.
   */
  void on_device_removed(const QString &id);

private:
  QList<QPointer<DeviceProvider>> m_providers;
  QHash<QString, QPointer<SmartDevice>> m_devices;
  mqtt::async_client m_client;

  /**
   * @brief called after establishing connection and connecting to topics to
   * notify all providers for them to perfom discovery/polling
   */
  void notify_providers_connected();
};

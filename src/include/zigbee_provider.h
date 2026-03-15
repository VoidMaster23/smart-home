#pragma once

#include "device_provider.h"
#include "mqtt_capability.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <QSet>

/**
 * @brief Device provider for Zigbee devices via Zigbee2MQTT.
 * 
 * Handles discovery and state management for Zigbee devices by communicating
 * with a Zigbee2MQTT bridge over MQTT.
 */
class ZigbeeProvider : public DeviceProvider, public MqttMixin {
  Q_OBJECT
public:
  /**
   * @brief Construct a ZigbeeProvider.
   * @param client Reference to the MQTT client.
   * @param parent Optional QObject parent.
   */
  explicit ZigbeeProvider(mqtt::async_client &client, QObject *parent = nullptr);

  /**
   * @brief Handle an incoming MQTT message.
   * @param topic Topic of the message.
   * @param payload Raw payload bytes.
   */
  void handle_message(const QString &topic, const QByteArray &payload) override;

  /**
   * @brief Poll all known devices for their current state.
   */
  void poll_all_devices() override;

  /**
   * @brief Called when the MQTT client connects to the broker.
   * 
   * Triggers device discovery by requesting the bridge's device list.
   */
  void on_connected() override;

private:
  /**
   * @brief Process a device discovery payload from the bridge.
   * @param devices JSON array of device descriptors.
   */
  void process_discovery(const QJsonArray &devices);

  /**
   * @brief Route a state update to a specific device.
   * @param friendly_name Friendly name of the device.
   * @param payload JSON object containing the update.
   */
  void route_update(const QString &friendly_name, const QJsonObject &payload);

  /**
   * @brief Extract IEEE addresses from a discovery payload.
   * @param devices JSON array of device descriptors.
   * @returns QSet of unique IEEE addresses.
   */
  [[nodiscard]] static QSet<QString> extract_snapshot_ids(const QJsonArray &devices) ;

  /**
   * @brief Register any new devices found in the discovery payload.
   * @param devices JSON array of device descriptors.
   */
  void register_new_devices(const QJsonArray &devices);

  /**
   * @brief Perform initial setup for a newly discovered device.
   * @param device Pointer to the SmartDevice.
   */
  void setup_new_device(const QPointer<SmartDevice> &device);

  /**
   * @brief Remove devices that are no longer present in the bridge's snapshot.
   * @param snapshot_ids Set of IEEE addresses currently present in the bridge.
   */
  void reconcile_missing_devices(const QSet<QString> &snapshot_ids);

protected:
  /**
   * @brief Mapping from device friendly names to their unique IEEE addresses.
   */
  QMap<QString, QString> m_name_to_id;
};

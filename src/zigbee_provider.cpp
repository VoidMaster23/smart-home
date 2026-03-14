#include "zigbee_provider.h"
#include "device_factory.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>

/**
 * @brief Create a Zigbee provider bound to an MQTT client and the
 * "zigbee2mqtt/" topic prefix.
 *
 * Initializes a provider that publishes and subscribes using the given MQTT
 * client under the "zigbee2mqtt/" topic namespace.
 *
 * @param client MQTT asynchronous client used for publishing and subscribing.
 * @param parent Optional QObject parent for ownership and lifetime management.
 */
ZigbeeProvider::ZigbeeProvider(mqtt::async_client &client, QObject *parent)
    : DeviceProvider(parent), MqttMixin(client, "zigbee2mqtt/") {}

/**
 * @brief Process an incoming MQTT message and route it to the appropriate
 * handler.
 *
 * Inspects the MQTT topic and either ignores irrelevant topics, handles a
 * bridge discovery message, or forwards a device update to its route handler.
 *
 * @param topic Full MQTT topic string for the incoming message; must start with
 * the provider's MQTT topic prefix to be processed.
 * @param payload Raw byte array payload of the MQTT message.
 */
void ZigbeeProvider::handle_message(const QString &topic,
                                    const QByteArray &payload) {
  if (!topic.startsWith(mqtt_topic_prefix())) {
    return;
  }

  QString rel_topic = topic.mid(mqtt_topic_prefix().length());

  // Ignore outgoing commands (topics ending in "/set" for a device)
  if (rel_topic.endsWith("/set")) {
    return;
  }

  QJsonDocument doc = QJsonDocument::fromJson(payload);
  if (doc.isNull()) {
    return;
  }

  // Precise bridge namespace check
  if (rel_topic == "bridge/devices") {
    QJsonArray devices;
    if (doc.isArray()) {
        devices = doc.array();
    } else if (doc.isObject()) {
        devices = doc.object()["devices"].toArray();
    }

    if (!devices.isEmpty()) {
        process_discovery(devices);
        poll_all_devices();
    }
    return;
  }

  if (rel_topic == "bridge" || rel_topic.startsWith("bridge/")) {
    return;
  }

  // If it's not a command and not in the bridge namespace, it's a device update
  if (doc.isObject()) {
    route_update(rel_topic, doc.object());
  }
}
/**
 * @brief Processes a discovery payload from Zigbee2MQTT and registers new
 * devices.
 *
 * Iterates the provided array of device descriptors, ignoring coordinator
 * entries and entries without an `ieee_address` or already-registered devices.
 * For each newly created device the provider stores mappings (id → device, name
 * → id), emits `device_discovered`, connects the device's request_command
 * signal so that outgoing commands are published to the MQTT topic
 * "<prefix><device_name>/set", and attaches a cleanup handler that removes
 * mappings and emits `device_removed` when the device is destroyed.
 *
 * @param devices Array of device objects as received from the Zigbee2MQTT
 *                bridge (each object is expected to contain an
 *                `ieee_address` and a `name`/`type` field).
 */
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
              [this, device](QJsonObject command) {
                if (command.contains("state") && command["state"].isBool()) {
                  command["state"] = command["state"].toBool() ? "ON" : "OFF";
                }
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

/**
 * @brief Routes an incoming payload to the device identified by its friendly
 * name.
 *
 * If the friendly name maps to a known device and that device exists, forwards
 * the payload to the device's update handler; otherwise does nothing.
 *
 * @param friendly_name Friendly name of the target device (as derived from the
 * MQTT topic).
 * @param payload JSON payload to deliver to the device.
 */
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

/**
 * @brief Requests Zigbee2MQTT to enumerate devices by publishing to the bridge
 * devices topic.
 *
 * Publishes an empty payload to "<prefix>bridge/devices" via the provider's
 * MQTT client to trigger device discovery.
 */
void ZigbeeProvider::on_connected() {
  QString topic = mqtt_topic_prefix() + "bridge/devices";
  mqtt_client()->publish(mqtt::make_message(topic.toStdString(), "", 1, false));
}

/**
 * @brief Requests state updates from every known Zigbee device.
 *
 * Iterates stored devices and publishes a {"state": ""} get payload to each
 * device's MQTT "<prefix><device_name>/get" topic; null entries are skipped.
 */
void ZigbeeProvider::poll_all_devices() {
  for (const auto &device : m_devices) {
    if (device == nullptr) {
      continue;
    }
    QString topic = mqtt_topic_prefix() + device->name() + "/get";
    QString payload = R"({"state": ""})";
    mqtt::message_ptr pubmsg =
        mqtt::make_message(topic.toStdString(), payload.toStdString());

    mqtt_client()->publish(pubmsg);
  }
}
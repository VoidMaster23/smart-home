#pragma once

#include <QString>
#include <concepts>
#include <utility>
#include <mqtt/async_client.h>

template <typename T>
concept MqttProvider = requires(T p) {
  { p.mqtt_client() } -> std::same_as<mqtt::async_client*>;
  { p.mqtt_topic_prefix() } -> std::same_as<QString>;
};

/**
 * MqttMixin provides a reusable mixin that stores a non-owning reference to an MQTT
 * asynchronous client and a topic prefix, exposing accessors for derived classes.
 */

/**
 * Construct an MqttMixin with a reference to an existing MQTT client and a topic prefix.
 * @param client Reference to an existing mqtt::async_client that the mixin will reference (non-owning).
 * @param prefix Topic prefix to store for MQTT topics; ownership is transferred into the mixin.
 */

/**
 * Provide access to the stored MQTT asynchronous client.
 * @returns Pointer to the referenced mqtt::async_client.
 */

/**
 * Access the stored MQTT topic prefix.
 * @returns The QString topic prefix.
 */
class MqttMixin {
public:
  MqttMixin(mqtt::async_client &client, QString prefix)
      : client(client), topic_prefix(std::move(prefix)) {}

  mqtt::async_client* mqtt_client() { return &client; }

  [[nodiscard]] QString mqtt_topic_prefix() const { return topic_prefix; }

  protected:
    mqtt::async_client &client;
    QString topic_prefix;
};
#pragma once

#include <QString>
#include <concepts>
#include <utility>
#include <mqtt/async_client.h>

/**
 * @brief Concept for a provider that uses MQTT.
 */
template <typename T>
concept MqttProvider = requires(T p) {
  { p.mqtt_client() } -> std::same_as<mqtt::async_client*>;
  { p.mqtt_topic_prefix() } -> std::same_as<const QString&>;
};

/**
 * @brief Mixin that provides MQTT client and topic prefix capabilities.
 * 
 * MqttMixin provides a reusable mixin that stores a non-owning reference to an MQTT
 * asynchronous client and a topic prefix, exposing accessors for derived classes.
 */
class MqttMixin {
public:
  /**
   * @brief Construct an MqttMixin.
   * @param client Reference to an existing mqtt::async_client (non-owning).
   * @param prefix Topic prefix to store for MQTT topics.
   */
  MqttMixin(mqtt::async_client &client, QString prefix)
      : client(client), topic_prefix(std::move(prefix)) {}

  /**
   * @brief Provide access to the stored MQTT asynchronous client.
   * @returns Pointer to the referenced mqtt::async_client.
   */
  mqtt::async_client* mqtt_client() { return &client; }

  /**
   * @brief Access the stored MQTT topic prefix.
   * @returns a const reference to the topic prefix.
   */
  [[nodiscard]] const QString& mqtt_topic_prefix() const { return topic_prefix; }

  protected:
    mqtt::async_client &client;
    QString topic_prefix;
};

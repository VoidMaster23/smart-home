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
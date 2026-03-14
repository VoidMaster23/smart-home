#include "zigbee_light.h"

#include <utility>

/**
 * @brief Constructs a ZigbeeLight with the given initialization parameters.
 *
 * @param params Initialization parameters used to configure the underlying SmartLight.
 */
ZigbeeLight::ZigbeeLight(SmartLightParams params): SmartLight(std::move(params)) {}

/**
 * @brief Set the light's brightness and propagate the change.
 *
 * Updates the internal brightness only if the new value differs from the current one,
 * emits the `brightness_changed` signal with the updated value, and publishes a JSON
 * payload containing the new brightness to the MQTT topic "zigbee2mqtt/<friendly_name()>/set"
 * via the `send_command` signal.
 *
 * @param brightness New brightness value.
 */
void ZigbeeLight::set_brightness(const int brightness) {
  if (brightness == this->m_brightness) {
    return;
  }

  this->m_brightness = brightness;
  emit this->brightness_changed(m_brightness);

  QString topic = "zigbee2mqtt/" + friendly_name() + "/set";
  QString payload = QString("{\"brightness\": %0}").arg(m_brightness);

  emit send_command(topic, payload);
}

/**
 * @brief Update the light's color temperature and publish the change via MQTT when different.
 *
 * Updates the stored color temperature, emits the `color_temp_changed` signal with the new value,
 * and sends an MQTT payload to the device topic to apply the new color temperature.
 *
 * @param color_temp New color temperature value.
 */
void ZigbeeLight::set_color_temp(const int color_temp) {
  if (color_temp == this->m_color_temp) {
    return;
  }

  this->m_color_temp = color_temp;
  emit this->color_temp_changed(m_color_temp);

  QString topic = "zigbee2mqtt/" + friendly_name() + "/set";
  QString payload = QString("{\"color_temp\": %1}").arg(m_color_temp);

  emit send_command(topic, payload);
}

/**
 * @brief Applies updates from a Zigbee MQTT payload to the light's state.
 *
 * Parses "state", "brightness", and "color_temp" fields from the provided JSON payload and updates
 * the light accordingly. "state" may be a string ("ON" treated as on) or a boolean; "brightness"
 * and "color_temp" are integer values and are applied if present.
 *
 * @param payload JSON object received from the Zigbee broker containing update fields.
 */
void ZigbeeLight::handle_update(QJsonObject const &payload) {
  auto state = payload["state"];
  auto brightness = payload["brightness"].toInt(-1);
  auto color_temp = payload["color_temp"].toInt(-1);

  if (state.isString()) {
    set_state(state.toString() == "ON");
  }

  if (state.isBool()) {
    set_state(state.toBool());
  }

  if (brightness != -1) {
    set_brightness(brightness);
  }

  if (color_temp != -1) {
    set_color_temp(color_temp);
  }
}
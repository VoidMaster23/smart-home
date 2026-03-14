#include "zigbee_light.h"

#include <utility>

/**
 * @brief Constructs a ZigbeeLight configured with the provided parameters.
 *
 * @param params Configuration parameters for the light; ownership of the supplied
 *               data is transferred to the constructed instance.
 */
ZigbeeLight::ZigbeeLight(SmartLightParams params): SmartLight(std::move(params)) {}

/**
 * @brief Update the light's brightness and notify listeners and external controller.
 *
 * Sets the internal brightness if it differs from the current value, emits the
 * `brightness_changed` signal, and publishes a JSON command to the Zigbee MQTT
 * topic "zigbee1mqtt/<friendly_name>/set" containing the new brightness.
 *
 * @param brightness New brightness value to apply.
 */
void ZigbeeLight::set_brightness(const int brightness) {
  if (brightness == this->m_brightness) {
    return;
  }

  this->m_brightness = brightness;
  emit this->brightness_changed(m_brightness);

  QString topic = "zigbee1mqtt/" + friendly_name() + "/set";
  QString payload = QString("{\"brightness\": %0}").arg(m_brightness);

  emit send_command(topic, payload);
}

/**
 * @brief Set the light's color temperature and publish the update to the Zigbee bridge.
 *
 * Updates the internal color temperature if different, emits the `color_temp_changed`
 * signal, and sends an MQTT command with the new color temperature to the device.
 *
 * @param color_temp Desired color temperature value to apply.
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
 * @brief Apply an incoming Zigbee update payload to the light's internal state.
 *
 * Updates the light's on/off state, brightness, and color temperature when the
 * corresponding fields are present in the provided JSON payload.
 *
 * @param payload JSON object received from Zigbee (e.g., MQTT) updates; may
 * contain:
 *  - "state": either a string ("ON"/other) or a boolean to set on/off state.
 *  - "brightness": integer brightness value (applied when present).
 *  - "color_temp": integer color temperature value (applied when present).
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
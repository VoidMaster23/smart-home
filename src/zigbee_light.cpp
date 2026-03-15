#include "zigbee_light.h"

#include <utility>

ZigbeeLight::ZigbeeLight(SmartLightParams params)
    : SmartLight(std::move(params)) {}

/**
 * @brief Set the light's brightness and emit an update command when it changes.
 *
 * Updates the internal brightness value if different from the current value and emits
 * a QJsonObject command with a "brightness" field via the request_command signal.
 *
 * @param brightness Desired brightness level to apply.
 */
void ZigbeeLight::set_brightness(const int brightness) {
  if (brightness == this->m_brightness) {
    return;
  }

  update_brightness(brightness);

  QJsonObject command;
  command["brightness"] = brightness;
  emit request_command(command);
}

/**
 * @brief Update the light's color temperature and request the change.
 *
 * If the provided value differs from the current color temperature, updates the internal
 * color temperature and emits request_command with a QJsonObject containing a
 * "color_temp" field set to the provided value.
 *
 * @param color_temp New color temperature value.
 */
void ZigbeeLight::set_color_temp(const int color_temp) {
  if (color_temp == this->m_color_temp) {
    return;
  }

  update_color_temp(color_temp);

  QJsonObject command;
  command["color_temp"] = color_temp;
  emit request_command(command);
}

/**
 * @brief Apply an incoming update payload to the light's internal state.
 *
 * Parses the provided JSON payload and updates the light's on/off state,
 * brightness, and color temperature when those fields are present.
 *
 * The `state` field may be either a string (expects "ON" for on) or a boolean.
 * The `brightness` and `color_temp` fields are interpreted as integers; absent
 * or missing values are ignored.
 *
 * @param payload JSON object that may contain keys "state", "brightness", and "color_temp".
 */
void ZigbeeLight::handle_update(QJsonObject const &payload) {
  auto state = payload["state"];
  auto brightness = payload["brightness"].toInt(-1);
  auto color_temp = payload["color_temp"].toInt(-1);

  if (state.isString()) {
    update_state(state.toString() == "ON");
  }

  if (state.isBool()) {
    update_state(state.toBool());
  }

  if (brightness != -1) {
    update_brightness(brightness);
  }

  if (color_temp != -1) {
    update_color_temp(color_temp);
  }
}
#include "zigbee_light.h"

#include <utility>

ZigbeeLight::ZigbeeLight(SmartLightParams params): SmartLight(std::move(params)) {}

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
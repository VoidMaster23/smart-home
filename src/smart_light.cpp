#include "smart_light.h"
#include "smart_device.h"
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <utility>

SmartLight::SmartLight(SmartLightParams params)
    : SmartDevice(std::move(params.id), std::move(params.name),
                  std::move(params.model), DeviceType::SmartLight,
                  params.parent),
      m_brightness(params.brightness), m_color_temp(params.color_temp),
      color_temp_range(params.color_temp_range) {}

int SmartLight::brightness() const { return this->m_brightness; }
int SmartLight::color_temp() const { return this->m_color_temp; }
ColorTempRange SmartLight::get_color_temp_range() const { return this->color_temp_range; }

void SmartLight::set_brightness(const int brightness) {
  if (brightness == this->m_brightness) {
    return;
  }

  this->m_brightness = brightness;
  emit this->brightness_changed(m_brightness);

  QString topic = "zigbee2mqtt/" + friendly_name() + "/set";
  QString payload = QString("{\"brightness\": %1}").arg(m_brightness);

  emit send_command(topic, payload);
}

void SmartLight::set_color_temp(const int color_temp) {
  if (color_temp == this->m_color_temp) {
    return;
  }

  this->m_color_temp = color_temp;
  emit this->color_temp_changed(m_color_temp);
}

void SmartLight::handle_update(QJsonObject const &payload) {
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

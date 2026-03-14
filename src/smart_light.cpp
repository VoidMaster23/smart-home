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
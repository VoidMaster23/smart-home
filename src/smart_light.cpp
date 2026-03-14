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

/**
 * @brief Retrieves the current brightness level of the smart light.
 *
 * @return int Current brightness level as stored by the device.
 */
int SmartLight::brightness() const { return this->m_brightness; }
/**
 * @brief Gets the current color temperature of the light.
 *
 * @return int The current color temperature value.
 */
int SmartLight::color_temp() const { return this->m_color_temp; }
/**
 * @brief Retrieves the supported color temperature range for this smart light.
 *
 * @return ColorTempRange The configured minimum and maximum color temperature supported by the device.
 */
ColorTempRange SmartLight::get_color_temp_range() const { return this->color_temp_range; }
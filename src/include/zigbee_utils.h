#pragma once

#include "smart_light.h"
#include <QJsonObject>

/**
 * Determine whether the provided JSON object represents a Zigbee device.
 * @param data JSON object containing device metadata and attributes.
 * @returns `true` if `data` corresponds to a Zigbee device, `false` otherwise.
 */
/**
 * Determine whether the provided JSON object represents a light device.
 * @param data JSON object containing device metadata and attributes.
 * @returns `true` if `data` describes a light-capable Zigbee device, `false` otherwise.
 */
/**
 * Extract the supported color temperature range for a light from the JSON object.
 * @param data JSON object containing light capabilities and configuration.
 * @returns A `ColorTempRange` describing the minimum and maximum color temperature supported by the device.
 */
/**
 * Parse light-specific parameters from the provided JSON object into a SmartLightParams structure.
 * @param data JSON object containing light state, capabilities, and configuration.
 * @returns A `SmartLightParams` instance populated from `data`.
 */
namespace ZigbeeUtils {
    bool is_zigbee_device(const QJsonObject &data);
    bool is_light(const QJsonObject &data);
    ColorTempRange get_color_temp_range(const QJsonObject &data);
    SmartLightParams parse_light_params(const QJsonObject &data);
}

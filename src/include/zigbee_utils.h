#pragma once

#include "smart_light.h"
#include <QJsonObject>
#include <optional>

/**
 * Determine whether a JSON object represents a Zigbee device.
 *
 * @param data JSON object containing device information (attributes, endpoints, model identifiers, etc.).
 * @returns `true` if the object appears to describe a Zigbee device, `false` otherwise.
 */
/**
 * Determine whether a Zigbee device JSON object represents a light.
 *
 * @param data JSON object containing device information (attributes, clusters, or model identifiers used to detect lights).
 * @returns `true` if the object appears to represent a light device, `false` otherwise.
 */
/**
 * Extract the supported color temperature range from a Zigbee device JSON object, if present.
 *
 * @param data JSON object containing device information (clusters, capabilities, or manufacturer-provided ranges).
 * @returns An optional ColorTempRange containing the minimum and maximum color temperature if detectable, empty otherwise.
 */
/**
 * Parse smart-light-specific parameters from a Zigbee device JSON object.
 *
 * @param data JSON object containing device information (state, capabilities, color/brightness values, and relevant clusters).
 * @returns A SmartLightParams object populated from the provided JSON data.
 */
namespace ZigbeeUtils {
    bool is_zigbee_device(const QJsonObject &data);
    bool is_light(const QJsonObject &data);
    std::optional<ColorTempRange> get_color_temp_range(const QJsonObject &data);
    SmartLightParams parse_light_params(const QJsonObject &data);
}

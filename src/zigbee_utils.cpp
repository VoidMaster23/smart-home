#include "zigbee_utils.h"
#include <QJsonArray>
#include <QJsonObject>
#include <optional>
#include <algorithm>

namespace ZigbeeUtils {

/**
 * Determines whether a JSON object represents a Zigbee device by checking for an IEEE address field.
 *
 * @param data JSON object containing device data.
 * @return true if `data` contains an "ieee_address" key whose value is a string, false otherwise.
 */
bool is_zigbee_device(const QJsonObject &data) {
  return data["ieee_address"].isString();
}

/**
 * @brief Determines whether a device JSON describes a light.
 *
 * @param data JSON object containing a device description; expected to include a "definition" object with an "exposes" array.
 * @return true if any entry in definition["exposes"] is an object whose "type" equals "light", false otherwise.
 */
bool is_light(const QJsonObject &data) {
  const QJsonValue definition = data["definition"];
  const QJsonValue exposes = definition["exposes"];

  if (!definition.isObject() || !exposes.isArray()) {
    return false;
  }

  const QJsonArray exposes_arr = exposes.toArray();
  return std::ranges::any_of(exposes_arr.cbegin(), exposes_arr.cend(),
                             [](const QJsonValue &entry) {
                               if (!entry.isObject()) {
                                 return false;
                               }

                               return entry["type"] == "light";
                             });
}

/**
 * @brief Extracts the color temperature range from a Zigbee device definition.
 *
 * @param device QJsonObject representing a Zigbee device; the function reads
 *               `device["definition"]["exposes"]` and looks for a "light"
 *               expose containing a feature named "color_temp".
 * @return std::optional<ColorTempRange> A ColorTempRange with `min`, `max`, and
 *         `neutral` (midpoint) when a "color_temp" feature is found;
 *         `std::nullopt` if no light expose or no "color_temp" feature exists.
 */
std::optional<ColorTempRange> getColorTempRange(const QJsonObject &device) {
  const auto exposes = device["definition"].toObject()["exposes"].toArray();

  auto it = std::ranges::find_if(exposes, [](const QJsonValue &val) {
    return val.toObject()["type"].toString() == "light";
  });

  if (it == exposes.end()) {
    return std::nullopt;
  }

  const auto features = it->toObject()["features"].toArray();

  auto features_iterator =
      std::ranges::find_if(features, [](const QJsonValue &val) {
        return val.toObject()["name"].toString() == "color_temp";
      });

  if (features_iterator == features.end()) {
    return std::nullopt;
  }

  const auto features_object = features_iterator->toObject();
  int min = features_object["value_min"].toInt();
  int max = features_object["value_max"].toInt();
  int neutral = min + ((max - min) / 2);
  return ColorTempRange{.min = min, .max = max, .neutral = neutral};
}

/**
 * @brief Builds SmartLightParams from a Zigbee device JSON object.
 *
 * Extracts the device's friendly name, IEEE address, model ID, and color temperature
 * range (falls back to a default range if not present) and returns a populated
 * SmartLightParams instance.
 *
 * @param data JSON object representing a Zigbee device; expected to contain
 *             keys "friendly_name", "ieee_address", and "model_id", and optionally
 *             a "definition" -> "exposes" structure for color temperature.
 * @return SmartLightParams Struct with fields:
 *         - id: the device's IEEE address,
 *         - name: the device's friendly name,
 *         - model: the device's model ID,
 *         - color_temp_range: extracted color temperature range or a default.
 */
SmartLightParams parse_light_params(const QJsonObject &data) {
  auto friendly_name = data["friendly_name"].toString();
  auto ieee_address = data["ieee_address"].toString();
  auto model_id = data["model_id"].toString();
  auto color_temp_range = getColorTempRange(data).value_or(ColorTempRange{});

  return SmartLightParams{
      .id = ieee_address,
      .name = friendly_name,
      .model = model_id,
      .color_temp_range = color_temp_range,
  };
}
} // namespace ZigbeeUtils
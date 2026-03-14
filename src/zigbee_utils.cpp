#include "zigbee_utils.h"
#include <QJsonArray>
#include <QJsonObject>
#include <optional>
#include <algorithm>

namespace ZigbeeUtils {

/**
 * @brief Determines whether the given JSON object represents a Zigbee device.
 *
 * Checks for the presence of an "ieee_address" field that is a string.
 *
 * @param data JSON object to inspect (expected device representation).
 * @return true if `data["ieee_address"]` is a string, `false` otherwise.
 */
bool is_zigbee_device(const QJsonObject &data) {
  return data["ieee_address"].isString();
}

/**
 * @brief Determine whether a JSON device object represents a light.
 *
 * Checks the object's "definition.exposes" array for an entry whose "type" equals "light".
 *
 * @param data JSON object representing a Zigbee device.
 * @return `true` if the device's definition exposes include an entry with `"type" == "light"`, `false` otherwise.
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
 * @brief Extracts the color temperature range for a Zigbee light definition.
 *
 * @param data QJsonObject representing a Zigbee device definition (expected to contain "definition" -> "exposes").
 * @return std::optional<ColorTempRange> Contains a ColorTempRange with `min`, `max`, and `neutral` (calculated as min + ((max - min) / 2)) when a "light" exposes entry with a "color_temp" feature is present; `std::nullopt` otherwise.
 */
std::optional<ColorTempRange> get_color_temp_range(const QJsonObject &data) {
  const auto exposes = data["definition"].toObject()["exposes"].toArray();

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
 * @brief Constructs SmartLightParams from a Zigbee device JSON object.
 *
 * Extracts the device's friendly name, IEEE address, model identifier, and color
 * temperature range (if present) and returns a populated SmartLightParams.
 *
 * @param data QJsonObject containing the Zigbee device representation.
 *             Expected keys: "friendly_name", "ieee_address", "model_id".
 * @return SmartLightParams Populated parameters:
 *         - id: IEEE address from `data["ieee_address"]`.
 *         - name: Friendly name from `data["friendly_name"]`.
 *         - model: Model identifier from `data["model_id"]`.
 *         - color_temp_range: Extracted color temperature range if available;
 *           otherwise a default-initialized ColorTempRange.
 */
SmartLightParams parse_light_params(const QJsonObject &data) {
  auto friendly_name = data["friendly_name"].toString();
  auto ieee_address = data["ieee_address"].toString();
  auto model_id = data["model_id"].toString();
  auto color_temp_range = get_color_temp_range(data).value_or(ColorTempRange{});

  return SmartLightParams{
      .id = ieee_address,
      .name = friendly_name,
      .model = model_id,
      .color_temp_range = color_temp_range,
  };
}
} // namespace ZigbeeUtils
#include "zigbee_utils.h"
#include <QJsonArray>
#include <QJsonObject>
#include <optional>
#include <algorithm>

namespace ZigbeeUtils {

bool is_zigbee_device(const QJsonObject &data) {
  return data["ieee_address"].isString();
}

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
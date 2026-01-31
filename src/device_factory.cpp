#include "smart_device.h"
#include "smart_light.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <algorithm>

namespace DeviceFactory {
bool is_light(QJsonObject const &device) {
  const QJsonValue definition = device["definition"];
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

// TODO: this will not work for devices I connect via MQTT exclusively, but I
// can also just force those devices to use this format when they join the
// network, maybe lol idk
SmartDevice *create_device(QJsonObject const &data) {
  auto friendly_name = data["friendly_name"].toString();
  auto ieee_address = data["ieee_address"].toString();
  auto model_id = data["model_id"].toString();

  if (is_light(data)) {
    SmartLightParams params {
      .id = ieee_address,
      .name = friendly_name,
      .model = model_id,
    };
    auto *light = new SmartLight(params);
    return light;
  }

  return nullptr;
}

} // namespace DeviceFactory
#pragma once

#include "smart_light.h"
#include <QJsonObject>

namespace ZigbeeUtils {
    bool is_zigbee_device(const QJsonObject &data);
    bool is_light(const QJsonObject &data);
    ColorTempRange get_color_temp_range(const QJsonObject &data);
    SmartLightParams parse_light_params(const QJsonObject &data);
}

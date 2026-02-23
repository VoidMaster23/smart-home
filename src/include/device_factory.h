#pragma once

#include "smart_device.h"
#include "smart_light.h"
#include <QObject>
#include <QString>
#include <QJsonObject>

#include <optional>

namespace DeviceFactory {
    SmartDevice* create_device(QJsonObject const &data);

    bool is_light(QJsonObject const &device);
    std::optional<ColorTempRange> getColorTempRange(const QJsonObject& device);
}
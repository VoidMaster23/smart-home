#pragma once

#include "smart_device.h"
#include <QObject>
#include <QString>
#include <QJsonObject>

namespace DeviceFactory {
    SmartDevice* create_device(QJsonObject const &data);

    bool is_light(QJsonObject const &device);
}
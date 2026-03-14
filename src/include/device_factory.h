#pragma once

#include "smart_device.h"
#include <QJsonObject>

#include <QPointer>

namespace DeviceFactory {
    QPointer<SmartDevice> create_device(QJsonObject const &data);

}
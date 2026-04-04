#pragma once

#include "smart_device.h"
#include <QJsonObject>
#include <QObject>
#include <QPointer>

namespace DeviceFactory {
    QPointer<SmartDevice> create_device(QJsonObject const &data, QObject *parent=nullptr);

}

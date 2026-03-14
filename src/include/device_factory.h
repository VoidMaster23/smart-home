#pragma once

#include "smart_device.h"
#include <QJsonObject>

#include <QPointer>

/**
 * Create a SmartDevice instance from a JSON device description.
 *
 * Constructs and initializes a SmartDevice according to the fields in `data`.
 *
 * @param data JSON object containing the device description and configuration (e.g., type and initialization properties).
 * @returns A QPointer to the created SmartDevice, or a null QPointer if creation fails or `data` is invalid.
 */
namespace DeviceFactory {
    QPointer<SmartDevice> create_device(QJsonObject const &data);

}
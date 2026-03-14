#pragma once

#include "smart_device.h"
#include <QJsonObject>

#include <QPointer>

/**
 * Create a SmartDevice instance from the provided JSON configuration.
 *
 * Parses the given QJsonObject to construct and initialize an appropriate SmartDevice subtype.
 * 
 * @param data JSON object containing the device's type and configuration fields.
 * @returns QPointer<SmartDevice> pointing to the created device, or a null QPointer if creation fails.
 */
namespace DeviceFactory {
    QPointer<SmartDevice> create_device(QJsonObject const &data);

}
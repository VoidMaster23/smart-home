#pragma once
#include "smart_light.h"

/**
 * Construct a ZigbeeLight with the given parameters.
 * @param params Initialization parameters for the smart light (model, id, defaults).
 */

/**
 * Process an incoming device update payload and apply any state changes to this light.
 * @param payload JSON object received from the device containing state and attribute fields.
 */

/**
 * Set the light's brightness level.
 * @param brightness Brightness value where higher numbers indicate increased brightness (device units, typically 0-100).
 */

/**
 * Set the light's color temperature.
 * @param color_temp Color temperature in Kelvin.
class ZigbeeLight: public SmartLight {
    Q_OBJECT

    public:
        explicit ZigbeeLight(SmartLightParams params);

        void handle_update(QJsonObject const &payload) override;

        void set_brightness(int brightness) override;
        void set_color_temp(int color_temp) override;
};
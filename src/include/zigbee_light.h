#pragma once
#include "smart_light.h"

/**
 * Zigbee-specific implementation of SmartLight.
 *
 * Provides handlers and mutators for updates and settings coming from or sent to Zigbee devices.
 */
 
/**
 * Construct a ZigbeeLight with the provided parameters.
 *
 * @param params Initialization parameters for the smart light.
 */
 
/**
 * Process an incoming JSON payload and update the light's internal state accordingly.
 *
 * @param payload JSON object containing update fields (for example: brightness, color temperature, or on/off state).
 */
 
/**
 * Set the light's brightness level.
 *
 * @param brightness Desired brightness level; higher values increase luminance.
 */
 
/**
 * Set the light's color temperature.
 *
 * @param color_temp Desired color temperature value (device-specific units).
 */
class ZigbeeLight: public SmartLight {
    Q_OBJECT

    public:
        explicit ZigbeeLight(SmartLightParams params);

        void handle_update(QJsonObject const &payload) override;

        void set_brightness(int brightness) override;
        void set_color_temp(int color_temp) override;
};
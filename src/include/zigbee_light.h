#pragma once
#include "smart_light.h"

/**
 * @brief Represents a Zigbee smart light.
 * 
 * Implements Zigbee-specific state updates and command creation.
 */
class ZigbeeLight: public SmartLight {
    Q_OBJECT

    public:
        /**
         * @brief Construct a ZigbeeLight.
         * @param params Construction parameters.
         */
        explicit ZigbeeLight(SmartLightParams params);

        /**
         * @brief Handle an incoming JSON payload from Zigbee2MQTT.
         * @param payload JSON object containing the device state.
         */
        void handle_update(QJsonObject const &payload) override;

        /**
         * @brief Set the brightness level and emit a Zigbee-specific command.
         * @param brightness Level to set (0-254).
         */
        void set_brightness(int brightness) override;

        /**
         * @brief Set the color temperature and emit a Zigbee-specific command.
         * @param color_temp Temperature in mireds.
         */
        void set_color_temp(int color_temp) override;
};

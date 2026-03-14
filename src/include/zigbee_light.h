#pragma once
#include "smart_light.h"

class ZigbeeLight: public SmartLight {
    Q_OBJECT

    public:
        explicit ZigbeeLight(SmartLightParams params);

        void handle_update(QJsonObject const &payload) override;

        void set_brightness(int brightness) override;
        void set_color_temp(int color_temp) override;
};
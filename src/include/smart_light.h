#pragma once

#include "smart_device.h"
#include <QObject>
#include <QString>
#include <QJsonObject>

struct SmartLightParams {
    QString id;
    QString name;
    QString model;
int color_temp{454}; //TODO: this will beb dynamic
    int brightness{};
    QObject *parent = nullptr;
};

class SmartLight : public SmartDevice {
    Q_OBJECT
    Q_PROPERTY(int brightness READ brightness WRITE set_brightness NOTIFY brightness_changed)
    Q_PROPERTY(int color_temp READ color_temp WRITE set_color_temp NOTIFY color_temp_changed)

    public:
    explicit SmartLight(SmartLightParams params);

    [[nodiscard]] int brightness() const;
    [[nodiscard]] int color_temp() const;

    void set_brightness( int brightness);
    void set_color_temp( int color_temp);

    void handle_update(QJsonObject const &payload) override;
    
    static constexpr int min_brightness = 0;
    static constexpr int max_brightness = 254;

    signals:
    void brightness_changed(int brightness);
    void color_temp_changed(int color_temp);
    
    private:
    int m_brightness;
    int m_color_temp;
};
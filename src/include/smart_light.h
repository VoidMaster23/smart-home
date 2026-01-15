#pragma once

#include "smart_device.h"
#include <QObject>
#include <QString>
#include <QJsonObject>

class SmartLight : public SmartDevice {
    Q_OBJECT
    Q_PROPERTY(int brightness READ brightness WRITE set_brightness NOTIFY brightness_changed)
    Q_PROPERTY(int color_temp READ color_temp WRITE set_color_temp NOTIFY color_temp_changed)

    public:
    SmartLight(QString id, QString name, QString model, QObject *parent = nullptr);

    [[nodiscard]] int brightness() const;
    [[nodiscard]] int color_temp() const;

    void set_brightness( int brightness);
    void set_color_temp( int color_temp);

    void handle_update(QJsonObject const &payload) override;

    signals:
        void brightness_changed();
        void color_temp_changed();

    private:
        int m_brightness;
        int m_color_temp;
};
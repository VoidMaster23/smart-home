#pragma once

#include "smart_light.h"
#include <QObject>
#include <QList>
#include <QJsonObject>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QPushButton>

class SmartLightWidget : public QWidget {
    Q_OBJECT
    public:
        SmartLightWidget(SmartLight *device, QWidget *parent);

    private:
        SmartLight * m_light_device;
        QLabel* m_name_label;
        QSlider *m_brightness_slider;
        QPushButton *m_toggle_button;
        QPushButton *btn_reduce_brightness;
        QPushButton *btn_increase_brightness;

        void setup_connections();



};
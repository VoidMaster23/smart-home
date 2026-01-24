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

    public:
        SmartLightWidget(SmartLight *device, QWidget *parent);

    private:
        SmartLight * m_light_device;
        QLabel* m_name_label;
        QSlider *m_brightness_slider;
        QPushButton *m_toggle_button;

        void setup_connections();



};
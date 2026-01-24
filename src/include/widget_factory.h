#pragma once

#include "smart_device.h"
#include <QtWidgets/QWidget>

namespace WidgetFactory {
    QWidget *create_widget(SmartDevice *device, QWidget *parent);
}
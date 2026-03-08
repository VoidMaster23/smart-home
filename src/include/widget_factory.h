#pragma once

#include "smart_device.h"
#include <QtWidgets/QWidget>
#include <QPointer>

namespace WidgetFactory {
    QPointer<QWidget> create_widget(const QPointer<SmartDevice>& device, const QPointer<QWidget>& parent);
}
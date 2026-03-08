#include "smart_device.h"
#include "smart_light_widget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QtWidgets/QWidget>
#include <QPointer>

#include <algorithm>

namespace WidgetFactory {
QPointer<QWidget> create_widget(const QPointer<SmartDevice>& device, const QPointer<QWidget>& parent) {
  if (device.isNull()) {
    return nullptr;
  }

  if (device->device_type() == DeviceType::SmartLight) {
    auto *light = qobject_cast<SmartLight *>(device.data());
    if(light != nullptr) {
        return new SmartLightWidget(light, parent); //NOLINT
    }
  }

  return nullptr;
}
} // namespace WidgetFactory
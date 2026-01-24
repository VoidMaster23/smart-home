#include "smart_device.h"
#include "smart_light_widget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QtWidgets/QWidget>

#include <algorithm>

namespace WidgetFactory {
QWidget *create_widget(SmartDevice *device, QWidget *parent) {
  if (device == nullptr) {
    return nullptr;
  }

  if (device->device_type() == DeviceType::SmartLight) {
    auto *light = dynamic_cast<SmartLight *>(device);
    if(light != nullptr) {
        return new SmartLightWidget(light, parent); //NOLINT
    }
  }

  return nullptr;
}
} // namespace WidgetFactory
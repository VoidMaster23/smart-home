#include "smart_device.h"
#include "zigbee_light.h"
#include "zigbee_utils.h"

#include <QObject>

namespace DeviceFactory {

QPointer<SmartDevice> create_device(QJsonObject const &data, QObject *parent=nullptr) {
 

  if (ZigbeeUtils::is_zigbee_device(data)) {
      if(ZigbeeUtils::is_light(data)) {
        auto params = ZigbeeUtils::parse_light_params(data);
        params.parent = parent;
        return {new ZigbeeLight(params)};
      }
  }

  return nullptr;
}

} // namespace DeviceFactory

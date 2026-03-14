#include "smart_device.h"
#include "zigbee_light.h"
#include "zigbee_utils.h"

namespace DeviceFactory {

QPointer<SmartDevice> create_device(QJsonObject const &data) {
 

  if (ZigbeeUtils::is_zigbee_device(data)) {
      if(ZigbeeUtils::is_light(data)) {
        return {new ZigbeeLight(ZigbeeUtils::parse_light_params(data))};
      }
  }

  return nullptr;
}

} // namespace DeviceFactory
#include "smart_device.h"
#include "zigbee_light.h"
#include "zigbee_utils.h"

namespace DeviceFactory {

/**
 * @brief Create a device instance from a JSON description when it represents a supported Zigbee light.
 *
 * @param data JSON object describing the device.
 * @return QPointer<SmartDevice> Pointer to a newly created ZigbeeLight when `data` describes a Zigbee light, `nullptr` otherwise.
 */
QPointer<SmartDevice> create_device(QJsonObject const &data) {
 

  if (ZigbeeUtils::is_zigbee_device(data)) {
      if(ZigbeeUtils::is_light(data)) {
        return {new ZigbeeLight(ZigbeeUtils::parse_light_params(data))};
      }
  }

  return nullptr;
}

} // namespace DeviceFactory
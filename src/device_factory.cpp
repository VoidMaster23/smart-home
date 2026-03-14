#include "smart_device.h"
#include "zigbee_light.h"
#include "zigbee_utils.h"

namespace DeviceFactory {

/**
 * @brief Create a SmartDevice instance from JSON data when it represents a Zigbee light.
 *
 * Detects Zigbee device information in the provided JSON and, if the object represents a Zigbee light,
 * constructs a new ZigbeeLight from parsed light parameters and returns it as a QPointer<SmartDevice>.
 *
 * @param data JSON object describing the device.
 * @return QPointer<SmartDevice> Pointer to a newly created ZigbeeLight upcast to SmartDevice when `data`
 *         describes a Zigbee light, `nullptr` otherwise.
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
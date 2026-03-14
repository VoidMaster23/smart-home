#include "smart_device.h"
#include "utils.h"

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <utility>

/**
 * @brief Construct a SmartDevice with identifying and descriptive properties.
 *
 * Initializes a SmartDevice's id, name, model, device type, and QObject parent; computes the device's display name from the provided name and initializes its state to false.
 *
 * @param id Unique device identifier (e.g., IEEE address).
 * @param name Human-readable device name.
 * @param model Device model identifier.
 * @param type DeviceType enum value describing the device category.
 * @param parent Optional QObject parent for ownership. 
 */
SmartDevice::SmartDevice(QString id, QString name, QString model,
                         DeviceType type, QObject *parent)
    : QObject(parent), m_name(std::move(name)),
      m_model(std::move(model)), m_id(std::move(id)), type(type),
      m_state(false) {
  m_display_name = StringUtils::format_for_display(m_name);
}

/**
 * @brief Retrieve the device's configured name.
 *
 * @return QString The device name.
 */
QString SmartDevice::name() const { return this->m_name; }

/**
 * @brief Get the device model identifier.
 *
 * @return QString The device model identifier.
 */
QString SmartDevice::model() const { return this->m_model; }
/**
 * @brief Retrieves the device's unique identifier (IEEE address).
 *
 * @return QString The device identifier.
 */
QString SmartDevice::id() const { return this->m_id; }
/**
 * @brief Retrieve the device's type.
 *
 * @return DeviceType The enum value representing this device's type.
 */
DeviceType SmartDevice::device_type() const { return this->type; }
/**
 * @brief Human-friendly display name for the device.
 *
 * The display name is computed from the device name during construction (formatted for display).
 *
 * @return QString The formatted display name used for presentation.
 */
QString SmartDevice::display_name() const { return this->m_display_name; }

bool SmartDevice::state() const { return this->m_state; }

void SmartDevice::update_state(bool state) {
  if (state == m_state) {
    return;
  }

  m_state = state;
  emit this->state_changed(m_state);
}

/**
 * @brief Set the device's on/off state and request a command when the state changes.
 *
 * Updates the device state only if the provided state differs from the current state,
 * and emits the request_command signal with a QJsonObject containing the key
 * "state" mapped to a boolean value.
 *
 * @param state `true` to set the device to on, `false` to set it to off.
 */
void SmartDevice::set_state(bool state) {
  if (state == m_state) {
    return;
  }

  update_state(state);
  
  QJsonObject command;
  command["state"] = state;
  emit request_command(command);
}
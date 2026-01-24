#include "smart_device.h"

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <utility>

SmartDevice::SmartDevice(QString id, QString name, QString model,
                         DeviceType type, QObject *parent)
    : QObject(parent), m_friendly_name(std::move(name)),
      m_model_id(std::move(model)), m_ieee_address(std::move(id)), type(type),
      m_state(false) {}

QString SmartDevice::friendly_name() const { return this->m_friendly_name; };

QString SmartDevice::model_id() const { return this->m_model_id; }
QString SmartDevice::ieee_address() const { return this->m_ieee_address; }
DeviceType SmartDevice::device_type() const { return this->type; }

bool SmartDevice::state() const { return this->m_state; }

void SmartDevice::set_state(bool state) {
  if (state == m_state) {
    return;
  }

  m_state = state;
  emit this->state_changed(m_state);
  QString topic = "zigbee2mqtt/" + friendly_name() + "/set";

  QString state_str = state ? "ON" : "OFF";
  QString payload = QString("{\"state\": %1}").arg(state_str);
  emit send_command(topic, payload);
}
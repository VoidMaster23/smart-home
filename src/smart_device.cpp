#include "smart_device.h"
#include "utils.h"

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <utility>

SmartDevice::SmartDevice(QString id, QString name, QString model,
                         DeviceType type, QObject *parent)
    : QObject(parent), m_name(std::move(name)),
      m_model(std::move(model)), m_id(std::move(id)), type(type),
      m_state(false) {
  m_display_name = StringUtils::format_for_display(m_name);
}

QString SmartDevice::name() const { return this->m_name; }

QString SmartDevice::model() const { return this->m_model; }
QString SmartDevice::id() const { return this->m_id; }
DeviceType SmartDevice::device_type() const { return this->type; }
QString SmartDevice::display_name() const { return this->m_display_name; }

bool SmartDevice::state() const { return this->m_state; }

void SmartDevice::update_state(bool state) {
  if (state == m_state) {
    return;
  }

  m_state = state;
  emit this->state_changed(m_state);
}

void SmartDevice::set_state(bool state) {
  if (state == m_state) {
    return;
  }

  update_state(state);
  
  QJsonObject command;
  command["state"] = state ? "ON" : "OFF";
  emit request_command(command);
}
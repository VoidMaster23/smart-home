#include "smart_device.h"

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <iostream>

SmartDevice::SmartDevice(QString &id, QString &name, QObject *parent)
    : QObject(parent), m_friendly_name(name), m_device_id(id), m_state(false) {}

QString SmartDevice::friendly_name() const { return this->m_friendly_name; };

QString SmartDevice::device_id() const { return this->m_device_id; }

bool SmartDevice::state() const { return this->m_state; }

// TODO: will update this later when we establish connection to devices
void SmartDevice::update_from_json(QJsonObject const &data) {
  auto status = data.find("state");
  if (status == data.end()) {
    std::cout << "State does not exist in ths variable" << "\n";
    return;
  }

  std::cout << status.value().toString().toStdString()
            << " is the current value" << "\n";

  if (status.value().isBool()) {
    std::cout << "YAY IS A BIOOLEAN";
  }
}

void SmartDevice::set_state(bool state) {
  if (state == m_state) {
    return;
  }

  m_state = state;
  emit this->state_changed();
}
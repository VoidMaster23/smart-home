#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>


enum class DeviceType {
  SmartLight
};

class SmartDevice : public QObject {
  Q_OBJECT
  Q_PROPERTY(
      QString friendly_name READ friendly_name NOTIFY friendly_name_changed)
  Q_PROPERTY(QString ieee_address READ ieee_address CONSTANT)
  Q_PROPERTY(QString model_id READ model_id CONSTANT)
  Q_PROPERTY(bool state READ state WRITE set_state NOTIFY state_changed)

public:
  SmartDevice( QString id,  QString name, QString model, DeviceType type, QObject *parent = nullptr);

  [[nodiscard]] QString friendly_name() const;
  [[nodiscard]] QString model_id() const;
  [[nodiscard]] QString ieee_address() const;
  [[nodiscard]] bool state() const;
  [[nodiscard]] DeviceType device_type() const;
  [[nodiscard]] QString display_name() const;

  void set_state(bool state);

  virtual void handle_update(QJsonObject const  &payload) = 0;

  //For memory we specify the virtual destructure to ensure that derived classes are cleaned
  // when using polymorphism 
  virtual ~SmartDevice() = default;

  //but since we defined the destructor we need to also explicitly create the copy/move constructors
  // this would adhere to the rule of 5 since now the compiler will NOT create them by default
  Q_DISABLE_COPY_MOVE(SmartDevice)

signals:

  void state_changed(bool state);
  void friendly_name_changed();
  void send_command(QString topic, QString payload);

protected:
  void update_state(bool state);

private:
  QString m_friendly_name;
  QString m_model_id;
  QString m_ieee_address;
  QString m_display_name;
  DeviceType type;
  bool m_state;
};
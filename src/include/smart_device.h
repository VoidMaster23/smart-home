#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>

enum class DeviceType { SmartLight };

class SmartDevice : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString id READ id CONSTANT)
  Q_PROPERTY(QString name READ name NOTIFY name_changed)
  Q_PROPERTY(QString model READ model CONSTANT)
  Q_PROPERTY(QString display_name READ display_name CONSTANT)
  Q_PROPERTY(bool state READ state WRITE set_state NOTIFY state_changed)
public:
  SmartDevice( QString id,  QString name, QString model, DeviceType type, QObject *parent = nullptr);

  [[nodiscard]] QString name() const;
  [[nodiscard]] QString model() const;
  [[nodiscard]] QString id() const;
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
  void name_changed();
  
  void request_command(const QJsonObject &command);
protected:
  void update_state(bool state);

private:
  QString m_name;
  QString m_model;
  QString m_id;
  QString m_display_name;
  DeviceType type;
  bool m_state;
};
#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>


class SmartDevice : public QObject {
  Q_OBJECT
  Q_PROPERTY(
      QString friendly_name READ friendly_name NOTIFY friendly_name_changed)
  Q_PROPERTY(QString device_id READ device_id CONSTANT)
  Q_PROPERTY(bool state READ state WRITE set_state NOTIFY state_changed)

public:
  SmartDevice(QString &id, QString &name, QObject *parent = nullptr);

  void update_from_json(QJsonObject const &data);

  [[nodiscard]] QString friendly_name() const;
  [[nodiscard]] QString device_id() const;
  [[nodiscard]] bool state() const;

  void set_state(bool state);

signals:

  void state_changed();
  void friendly_name_changed();

private:
  QString m_friendly_name;
  QString m_device_id;
  bool m_state;
};
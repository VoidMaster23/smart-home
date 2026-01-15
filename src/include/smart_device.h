#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>


class SmartDevice : public QObject {
  Q_OBJECT
  Q_PROPERTY(
      QString friendly_name READ friendly_name NOTIFY friendly_name_changed)
  Q_PROPERTY(QString ieee_address READ ieee_address CONSTANT)
  Q_PROPERTY(QString model_id READ model_id CONSTANT)
  Q_PROPERTY(bool state READ state WRITE set_state NOTIFY state_changed)

public:
  SmartDevice( QString id,  QString name, QString model, QObject *parent = nullptr);

  [[nodiscard]] QString friendly_name() const;
  [[nodiscard]] QString model_id() const;
  [[nodiscard]] QString ieee_address() const;
  [[nodiscard]] bool state() const;

  void set_state(bool state);

  virtual void handle_update(QJsonObject const  &payload) = 0;

signals:

  void state_changed();
  void friendly_name_changed();

private:
  QString m_friendly_name;
  QString m_model_id;
  QString m_ieee_address;
  bool m_state;
};
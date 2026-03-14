#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>

/**
 * Construct a SmartDevice with identifiers, display properties, and device type.
 * @param id Device unique identifier (e.g., IEEE address or internal id).
 * @param name Human-friendly device name.
 * @param model Device model identifier.
 * @param type DeviceType enum value describing the device category.
 * @param parent Optional QObject parent.
 */
/**
 * Get the device name.
 * @returns The human-friendly name of the device.
 */
/**
 * Get the device model identifier.
 * @returns The model identifier string.
 */
/**
 * Get the device unique identifier.
 * @returns The device id string.
 */
/**
 * Get the device on/off state.
 * @returns `true` if the device is on, `false` otherwise.
 */
/**
 * Get the device type.
 * @returns The DeviceType value for this device.
 */
/**
 * Get the device display name.
 * @returns The display name used for UI presentation.
 */
/**
 * Set the device on/off state.
 * @param state `true` to mark the device as on, `false` to mark it as off.
 */
/**
 * Handle an incoming JSON payload to update device state or produce commands.
 * @param payload JSON object containing update information or command data relevant to the device.
 */
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
  DeviceType m_type;
  bool m_state;
};
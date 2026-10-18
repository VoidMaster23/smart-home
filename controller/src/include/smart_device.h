#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QtQml/qqmlregistration.h>
#include <qqmlintegration.h>

/**
 * @brief Enum representing the type of smart device.
 */
enum class DeviceType { SmartLight };

/**
 * @brief Base class for all smart devices in the system.
 */
class SmartDevice : public QObject {
  Q_OBJECT
  QML_UNCREATABLE("Base Class cannot be instantiated")
  Q_PROPERTY(QString id READ id CONSTANT)
  Q_PROPERTY(QString name READ name NOTIFY name_changed)
  Q_PROPERTY(QString model READ model CONSTANT)
  Q_PROPERTY(QString display_name READ display_name CONSTANT)
  Q_PROPERTY(bool state READ state WRITE set_state NOTIFY state_changed)
public:
  /**
   * @brief Construct a SmartDevice with identifiers, display properties, and device type.
   * @param id Device unique identifier (e.g., IEEE address or internal id).
   * @param name Human-friendly device name.
   * @param model Device model identifier.
   * @param type DeviceType enum value describing the device category.
   * @param parent Optional QObject parent.
   */
  SmartDevice( QString id,  QString name, QString model, DeviceType type, QObject *parent = nullptr);

  /**
   * @brief Get the device name.
   * @returns The human-friendly name of the device.
   */
  [[nodiscard]] QString name() const;

  /**
   * @brief Get the device model identifier.
   * @returns The model identifier string.
   */
  [[nodiscard]] QString model() const;

  /**
   * @brief Get the device unique identifier.
   * @returns The device id string.
   */
  [[nodiscard]] QString id() const;

  /**
   * @brief Get the device on/off state.
   * @returns `true` if the device is on, `false` otherwise.
   */
  [[nodiscard]] bool state() const;

  /**
   * @brief Get the device type.
   * @returns The DeviceType value for this device.
   */
  [[nodiscard]] DeviceType device_type() const;

  /**
   * @brief Get the device display name.
   * @returns The display name used for UI presentation.
   */
  [[nodiscard]] QString display_name() const;

  /**
   * @brief Set the device on/off state.
   * @param state `true` to mark the device as on, `false` to mark it as off.
   */
  void set_state(bool state);

  /**
   * @brief Handle an incoming JSON payload to update device state or produce commands.
   * @param payload JSON object containing update information or command data relevant to the device.
   */
  virtual void handle_update(QJsonObject const  &payload) = 0;

  /**
   * @brief Virtual destructor to ensure that derived classes are cleaned when using polymorphism.
   */
  virtual ~SmartDevice() = default;

  /**
   * @brief Disable copy and move constructors to adhere to the rule of 5.
   */
  Q_DISABLE_COPY_MOVE(SmartDevice)

signals:

  /**
   * @brief Emitted when the device state changes.
   * @param state New state of the device.
   */
  void state_changed(bool state);

  /**
   * @brief Emitted when the device name changes.
   */
  void name_changed();
  
  /**
   * @brief Emitted when a command is requested for the device.
   * @param command JSON object containing the command data.
   */
  void request_command(const QJsonObject &command);

protected:
  /**
   * @brief Internal method to update the device state without emitting a command.
   * @param state New state of the device.
   */
  void update_state(bool state);

private:
  QString m_name;
  QString m_model;
  QString m_id;
  QString m_display_name;
  DeviceType m_type;
  bool m_state;
};

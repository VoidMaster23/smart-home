#pragma once
#include "smart_device.h"

#include <QObject>
#include <QString>

/**
 * Abstract base class that discovers and tracks SmartDevice instances and
 * exposes lifecycle hooks for connection and message handling.
 */
 
/**
 * @brief Emitted when a device is discovered and becomes available.
 * @param device Pointer to the discovered SmartDevice.
 */
 
/**
 * @brief Emitted when a device is removed from tracking.
 * @param id Identifier of the removed device.
 */

/**
 * @brief Construct a DeviceProvider with an optional parent QObject.
 * @param parent Parent QObject, or nullptr.
 */

/**
 * @brief Virtual destructor.
 */

/**
 * @brief Handle an incoming message associated with a topic.
 * @param topic Topic or routing key for the message.
 * @param payload JSON payload of the message.
 */

/**
 * @brief Poll all known or discoverable devices to refresh state or discover new ones.
 */

/**
 * @brief Called when a connection has been established and the provider can begin communication.
 */
class DeviceProvider: public QObject {
    Q_OBJECT
    signals:
        void device_discovered(QPointer<SmartDevice> device);
        void device_removed(QString id);
    
    public: 
        explicit DeviceProvider(QObject *parent = nullptr): QObject(parent) {}
        virtual ~DeviceProvider() = default;

        Q_DISABLE_COPY_MOVE(DeviceProvider)

        virtual void handle_message(const QString &topic, const QJsonObject &payload) = 0;
        virtual void poll_all_devices() = 0;
        virtual void on_connected() = 0;
    
    protected:
        QMap<QString, QPointer<SmartDevice>> m_devices;
};
#pragma once
#include "smart_device.h"

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QPointer>
#include <QMap>

/**
 * @brief Abstract base class that discovers and tracks SmartDevice instances and
 * exposes lifecycle hooks for connection and message handling.
 */
class DeviceProvider: public QObject {
    Q_OBJECT
    signals:
        /**
         * @brief Emitted when a device is discovered and becomes available.
         * @param device Pointer to the discovered SmartDevice.
         */
        void device_discovered(QPointer<SmartDevice> device);

        /**
         * @brief Emitted when a device is removed from tracking.
         * @param id Identifier of the removed device.
         */
        void device_removed(QString id);
    
    public: 
        /**
         * @brief Construct a DeviceProvider with an optional parent QObject.
         * @param parent Parent QObject, or nullptr.
         */
        explicit DeviceProvider(QObject *parent = nullptr): QObject(parent) {}

        /**
         * @brief Virtual destructor.
         */
        virtual ~DeviceProvider() = default;

        /**
         * @brief Disable copy and move constructors.
         */
        Q_DISABLE_COPY_MOVE(DeviceProvider)

        /**
         * @brief Handle an incoming message associated with a topic.
         * @param topic Topic or routing key for the message.
         * @param payload Raw byte array payload of the message.
         */
        virtual void handle_message(const QString &topic, const QByteArray &payload) = 0;

        /**
         * @brief Poll all known or discoverable devices to refresh state or discover new ones.
         */
        virtual void poll_all_devices() = 0;

        /**
         * @brief Called when a connection has been established and the provider can begin communication.
         */
        virtual void on_connected() = 0;
    
    protected:
        /**
         * @brief Map of managed devices keyed by their unique identifier.
         */
        QMap<QString, QPointer<SmartDevice>> m_devices;
};

#pragma once
#include "smart_device.h"

#include <QObject>
#include <QString>

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
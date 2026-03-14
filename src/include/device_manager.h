#pragma once

#include <QPointer>
#include <QObject>
#include <QList>
#include <QJsonObject>
#include <mqtt/async_client.h>

#include "smart_device.h"
#include "device_provider.h"


class DeviceManager : public QObject, public virtual mqtt::callback {
    Q_OBJECT
    Q_PROPERTY(QList<QPointer<SmartDevice>> devices READ devices NOTIFY devices_changed)

    public:
        explicit DeviceManager(QObject *parent = nullptr);

        void register_provider(DeviceProvider* provider);
        mqtt::async_client& mqtt_client() { return m_client; }

        [[nodiscard]] QList<QPointer<SmartDevice>> devices() const;
        [[nodiscard]] QPointer<SmartDevice> get_device(const QString &id) const;

        void connection_lost(const std::string &cause) override;
        void message_arrived(mqtt::const_message_ptr msg) override;
        void delivery_complete(mqtt::delivery_token_ptr token) override;

        void connect_to_broker();

    signals:
    
     void devices_changed();
     void device_discovered(SmartDevice *device);

    private slots:
        void on_device_discovered(const QPointer<SmartDevice> &device);
        void on_device_removed(const QString &id);

    private:
        QList<QPointer<DeviceProvider>> m_providers;
        QHash<QString, QPointer<SmartDevice>> m_devices;
        mqtt::async_client m_client;   

};
#pragma once

#include <QPointer>
#include <QObject>
#include <QList>
#include <QJsonObject>
#include <mqtt/async_client.h>

#include <smart_device.h>


class DeviceManager : public QObject, public virtual mqtt::callback {
    Q_OBJECT
    Q_PROPERTY(QList<SmartDevice*> devices READ devices NOTIFY devices_changed)

    public:
        explicit DeviceManager(QObject *parent = nullptr);

        [[nodiscard]] QList<QPointer<SmartDevice>> devices() const;
        [[nodiscard]] QPointer<SmartDevice> get_device(QString &id) const;

        void handle_message(QString &topic , QJsonObject &payload);

        void connection_lost(const std::string &cause) override;
        void message_arrived(mqtt::const_message_ptr msg) override;
        void delivery_complete(mqtt::delivery_token_ptr token) override;
        void add_new_device(QJsonArray const &payload);        


        void connect_to_broker();

    signals:
    
     void devices_changed();
     void device_discovered(const QPointer<SmartDevice>& device);
    private:
        QHash<QString, QPointer<SmartDevice>> m_devices;
        mqtt::async_client m_client;   

};
#pragma once

#include "device_provider.h"
#include "mqtt_capability.h"
#include <QMap>

class ZigbeeProvider : public DeviceProvider, public MqttMixin {
    Q_OBJECT
    public:
        explicit ZigbeeProvider(mqtt::async_client &client, QObject *parent = nullptr);

    void handle_message(const QString &topic, const QJsonObject &payload) override;
    void poll_all_devices() override;
    void on_connected() override;
    
    private:
        void process_discovery(const QJsonArray &devices);
        void route_update(const QString &friendly_name, const QJsonObject &payload);
    
    protected:
        QMap<QString, QString> m_name_to_id;
};
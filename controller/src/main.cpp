#include "device_manager.h"
#include "zigbee_provider.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include <exception>

int main(int argc, char *argv[]) {
  try {
    QGuiApplication app(argc, argv);
    DeviceManager manager;

    auto *zigbee = new ZigbeeProvider(manager.mqtt_client(), // NOLINT
                                      &manager);             // NOLINT
    manager.register_provider(zigbee);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("manager", &manager);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("SmartHome", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    manager.connect_to_broker();

    qDebug() << "[SUCCESS] Qt App Initialized. Check your screen";

    return QGuiApplication::exec();
  } catch (const std::exception &e) {
    qCritical() << "[ERROR] Startup failed: " << e.what();
    return 1;
  }
}

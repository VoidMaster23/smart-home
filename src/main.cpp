#include <iostream>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <mqtt/async_client.h>

int main(int argc, char *argv[]) {
    try {
        mqtt::async_client client("tcp://localhost:1883", "test_client");
        std::cout << "WOO PAHO DOING DA TINGS" << std::endl;
    } catch (...) {
        std::cout << "FUUUUUCK NO PAHO" << std::endl;
    }

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    const QUrl url(u"qrc:/SmartHome/Main.qml"_qs);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
        
    engine.load(url);

    std::cout << "[SUCCESS] Qt App Initialized. Check your screen!" << std::endl;

    return app.exec();
}
#include "device_manager.h"

#include <iostream>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qqmlcontext.h>

int main(int argc, char *argv[]) {
  DeviceManager manager;
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;

  using namespace Qt::Literals::StringLiterals;
  const QUrl url(u"qrc:/SmartHome/src/Main.qml"_s);

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  engine.rootContext()->setContextProperty("backend", &manager);
  manager.connect_to_broker();

  engine.load(url);

  std::cout << "[SUCCESS] Qt App Initialized. Check your screen!" << '\n';

  return QGuiApplication::exec();
}
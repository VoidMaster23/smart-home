#include "device_manager.h"
#include "main_window.h"
#include "zigbee_provider.h"

#include <iostream>

#include <QApplication>
#include <QFile>
#include <QFileInfo>

#include <exception>

int main(int argc, char *argv[]) {
  try {
    QApplication app(argc, argv);
    DeviceManager manager;

    auto *zigbee = new ZigbeeProvider(manager.mqtt_client(), // NOLINT
                                      &manager);             // NOLINT
    manager.register_provider(zigbee);

    QFile file(style_path);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
      QTextStream stream(&file);
      app.setStyleSheet(stream.readAll());
      file.close();
    }

    MainWindow window(&manager);

    window.showFullScreen();

    manager.connect_to_broker();

    std::cout << "[SUCCESS] Qt App Initialized. Check your screen!" << '\n';

    return QApplication::exec();
  } catch (const std::exception &e) {
    std::cerr << "[ERROR] Startup failed" << e.what() << '\n';
    return 1;
  }
}
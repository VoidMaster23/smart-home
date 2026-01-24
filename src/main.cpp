#include "device_manager.h"
#include "main_window.h"

#include <iostream>

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  DeviceManager manager;

  MainWindow window(&manager);

  window.showFullScreen();

  manager.connect_to_broker();

  std::cout << "[SUCCESS] Qt App Initialized. Check your screen!" << '\n';

  return app.exec();
}
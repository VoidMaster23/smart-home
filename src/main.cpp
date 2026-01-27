#include "device_manager.h"
#include "main_window.h"

#include <iostream>

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QDir>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  DeviceManager manager;

  QFile file(style_path);
  if(file.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());
    file.close();
  }

  MainWindow window(&manager);

  window.showFullScreen();

  manager.connect_to_broker();

  std::cout << "[SUCCESS] Qt App Initialized. Check your screen!" << '\n';

  return QApplication::exec();
}
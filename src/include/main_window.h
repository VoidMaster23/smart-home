#pragma once
#include <QtWidgets/QMainWindow>

class DeviceManager;
class SmartDevice;
class QVBoxLayout;
class QWidget;

class MainWindow : public QMainWindow {
  Q_OBJECT

  public:
  MainWindow(DeviceManager *manager, QWidget *parent = nullptr);

  void on_device_found(SmartDevice *device);

  private:
  DeviceManager *manager;
  QWidget *scroll_content;
  QVBoxLayout *main_layout;

};
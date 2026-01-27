#pragma once
#include <QtWidgets/QMainWindow>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QDir>

const QString style_path = "src/style.qss";

class DeviceManager;
class SmartDevice;
class QVBoxLayout;
class QWidget;

class MainWindow : public QMainWindow {
  Q_OBJECT

  public:
  MainWindow(DeviceManager *manager, QWidget *parent = nullptr);

  void on_device_found(SmartDevice *device);
  static void update_style() ;

  private:
  DeviceManager *manager;
  QWidget *scroll_content;
  QVBoxLayout *main_layout;
  QFileSystemWatcher *watcher;


};
#pragma once
#include <QPointer>
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
  MainWindow(const QPointer<DeviceManager>& manager, QWidget *parent = nullptr);

  void on_device_found(const QPointer<SmartDevice>& device);
  static void update_style() ;

  private:
  QPointer<DeviceManager> manager;
  QPointer<QWidget> scroll_content;
  QPointer<QVBoxLayout> main_layout;
  QPointer<QFileSystemWatcher> watcher;


};
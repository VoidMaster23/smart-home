#include "main_window.h"
#include "device_manager.h"
#include "smart_device.h"
#include "widget_factory.h"

#include <QApplication>
#include <QFile>
#include <QFileSystemWatcher>
#include <QString>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

MainWindow::MainWindow(DeviceManager *manager, QWidget *parent)
    : QMainWindow(parent), manager(manager) {
  auto *scroll_area = new QScrollArea(this); // NOLINT
  scroll_area->setWidgetResizable(true);

  scroll_content = new QWidget();                // NOLINT
  main_layout = new QVBoxLayout(scroll_content); // NOLINT

  main_layout->addStretch();

  scroll_area->setWidget(scroll_content);
  this->setCentralWidget(scroll_area);


  watcher = new QFileSystemWatcher({style_path}, this); // NOLINT

  connect(manager, &DeviceManager::device_discovered, this,
          &MainWindow::on_device_found);

  connect(watcher, &QFileSystemWatcher::fileChanged, this,
          [this](const QString &path) {
            update_style();
            if (!watcher->files().contains(path)) {
              watcher->addPath(path);
            }
          });
  update_style();
}

void MainWindow::update_style() {
  std::cout << "[DEBUG] Trying to load style from: " << style_path.toStdString() << "\n";
  QFile file(style_path);
  if (file.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream stream(&file);
    qApp->setStyleSheet(stream.readAll());
    file.close();
  } else {
    std::cout << "[ERROR] file not found" << '\n';
  }
}

void MainWindow::on_device_found(SmartDevice *device) {
  auto *widget = WidgetFactory::create_widget(device, scroll_content);

  if (widget == nullptr) {
    return;
  }

  main_layout->insertWidget(main_layout->count() - 1, widget);
}

#pragma once

#include "smart_light.h"
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

class SmartLightWidget : public QWidget {
  Q_OBJECT
public:
  SmartLightWidget(const QPointer<SmartLight>& device, const QPointer<QWidget>& parent);

protected:
  void resizeEvent(QResizeEvent *event) override;

private:
  QPointer<SmartLight> m_light_device;
  QPointer<QLabel> m_name_label;
  QPointer<QSlider> m_brightness_slider;
  QPointer<QPushButton> m_toggle_button;
  QPointer<QPushButton> btn_reduce_brightness;
  QPointer<QPushButton> btn_increase_brightness;
  QPointer<QPushButton> btn_settings;

  void setup_connections();
  void update_slider_label_pos();
};
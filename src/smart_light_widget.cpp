#include "smart_light_widget.h"
#include "smart_light.h"
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QSignalBlocker>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSizePolicy>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

SmartLightWidget::SmartLightWidget(SmartLight *device, QWidget *parent)
    : QWidget(parent), m_light_device(device), m_brightness_slider() {

  auto *layout = new QHBoxLayout(this); // NOLINT

  m_name_label = new QLabel(); // NOLINT
  m_name_label->setText(device->friendly_name().replace('_', ' '));
  m_name_label->setAlignment(Qt::AlignVCenter);
  QFont font = m_name_label->font();
  font.setCapitalization(QFont::Capitalize);
  m_name_label->setFont(font);

  m_brightness_slider = new QSlider(); // NOLINT
  m_brightness_slider->setOrientation(Qt::Orientation::Horizontal);
  m_brightness_slider->setMinimumWidth(100);
  m_brightness_slider->setMinimumHeight(50);
  m_brightness_slider->setValue(device->brightness());

  m_toggle_button = new QPushButton(); // NOLINT
  m_toggle_button->setCheckable(true);
  m_toggle_button->setFixedSize(50, 50);
  auto device_state = device->state();
  m_toggle_button->setChecked(device_state);
  m_toggle_button->setText(device_state ? "ON" : "OFF");

  layout->addWidget(m_name_label);
  layout->addWidget(m_brightness_slider, 1);
  layout->addWidget(m_toggle_button);

  setup_connections();

  this->setAttribute(Qt::WA_StyledBackground, true);
}

void SmartLightWidget::setup_connections() {
  connect(m_brightness_slider, &QSlider::valueChanged, this,
          [this](int value) { m_light_device->set_brightness(value); });

  connect(m_toggle_button, &QPushButton::toggled, this,
          [this](bool is_checked) { m_light_device->set_state(is_checked); });

  connect(m_light_device, &SmartLight::brightness_changed, this,
          [this](int value) {
            const QSignalBlocker blocker(m_brightness_slider);
            m_brightness_slider->setValue(value);
          });

  connect(m_light_device, &SmartLight::state_changed, this,
          [this](bool is_checked) {
            const QSignalBlocker blocker(m_toggle_button);
            m_toggle_button->setChecked(is_checked);
            m_toggle_button->setText(is_checked ? "ON" : "OFF");
          });
}

#include "smart_light_widget.h"
#include "smart_light.h"
#include "smart_light_settings_dialog.h"
#include <QDebug>
#include <QIcon>
#include <QImageReader>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QSignalBlocker>
#include <QStyle>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSizePolicy>
#include <QtWidgets/QSlider>
#include <QtWidgets/QToolTip>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

void SmartLightWidget::update_slider_label_pos() {
  if (m_brightness_slider.isNull()) {
    return;
  }
  auto *label = this->findChild<QLabel *>("label");
  int brightness = this->m_brightness_slider->value();
  label->setText(QString::number(
      qRound(((double)brightness * 100) / SmartLight::max_brightness)));
  label->adjustSize();

  double ratio = (double)(brightness - this->m_brightness_slider->minimum()) /
                 (this->m_brightness_slider->maximum() -
                  this->m_brightness_slider->minimum());

  int x = qRound(this->m_brightness_slider->x() +
                 qRound(this->m_brightness_slider->width() * ratio) -
                 ((double)label->width() / 2));
  int y = this->m_brightness_slider->y() + 10;

  label->move(x, y);
}

SmartLightWidget::SmartLightWidget(const QPointer<SmartLight> &device,
                                   const QPointer<QWidget> &parent)
    : QWidget(parent.data()), m_light_device(device) {

  if (device.isNull()) {
    return;
  }

  auto *layout = new QHBoxLayout(this);                   // NOLINT
  auto *slider_container = new QHBoxLayout();             // NOLINT
  auto *container = new QVBoxLayout();                    // NOLINT
  auto *label_and_settings_container = new QHBoxLayout(); // NOLINT

  // label and settings container should be a flex layout
  label_and_settings_container->setProperty("type", "flex");

  // button label
  m_name_label = new QLabel(this); // NOLINT
  m_name_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  m_name_label->setText(device->display_name());
  m_name_label->setAlignment(Qt::AlignLeft);

  // settings button
  btn_settings = new QPushButton(this); // NOLINT
  btn_settings->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  QIcon::setThemeName("Adwaita");
  QIcon icon = QIcon::fromTheme("preferences-system-symbolic");

  btn_settings->setIcon(icon);
  btn_settings->setProperty("type", "settings_button");

  // slider control buttons
  btn_reduce_brightness = new QPushButton(this);   // NOLINT
  btn_increase_brightness = new QPushButton(this); // NOLINT
  btn_increase_brightness->setSizePolicy(QSizePolicy::Preferred,
                                         QSizePolicy::Fixed);
  btn_reduce_brightness->setSizePolicy(QSizePolicy::Preferred,
                                       QSizePolicy::Fixed);
  btn_increase_brightness->setProperty("type", "slider_control");
  btn_reduce_brightness->setProperty("type", "slider_control");
  btn_reduce_brightness->setText("<");
  btn_increase_brightness->setText(">");

  m_brightness_slider = new QSlider(this); // NOLINT
  m_brightness_slider->setOrientation(Qt::Orientation::Horizontal);
  m_brightness_slider->setMinimumWidth(100);
  m_brightness_slider->setMinimumHeight(4);
  m_brightness_slider->setValue(device->brightness());
  m_brightness_slider->setRange(SmartLight::min_brightness,
                                SmartLight::max_brightness);
  m_brightness_slider->setMaximumHeight(8);
  m_brightness_slider->setTickInterval(15);
  m_brightness_slider->setAttribute(Qt::WA_TransparentForMouseEvents);

  auto *brightness_label =
      new QLabel(QString::number(device->brightness()), this);
  brightness_label->adjustSize();
  brightness_label->setObjectName("label");

  m_toggle_button = new QPushButton(this); // NOLINT
  m_toggle_button->setCheckable(true);
  m_toggle_button->setFixedSize(50, 50);
  auto device_state = device->state();
  m_toggle_button->setChecked(device_state);
  m_toggle_button->setText(device_state ? "ON" : "OFF");

  // connect the shit together
  slider_container->addWidget(btn_reduce_brightness.data());
  slider_container->addWidget(m_brightness_slider.data(), 1);
  slider_container->addWidget(btn_increase_brightness.data());

  label_and_settings_container->addWidget(m_name_label.data());
  label_and_settings_container->addWidget(btn_settings.data());

  container->addLayout(label_and_settings_container);
  container->addLayout(slider_container, 1);
  // set the gap between elements
  container->setSpacing(4);

  layout->addLayout(container, 1);
  layout->addWidget(m_toggle_button.data());

  setup_connections();

  this->setAttribute(Qt::WA_StyledBackground, true);
}

void SmartLightWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  this->update_slider_label_pos();
}

void SmartLightWidget::setup_connections() {
  if (m_light_device.isNull()) {
    return;
  }

  connect(m_brightness_slider.data(), &QSlider::valueChanged, this,
          [this](int value) {
            if (m_light_device) {
              m_light_device->set_brightness(value);
              this->update_slider_label_pos();
            }
          });

  connect(m_toggle_button.data(), &QPushButton::toggled, this,
          [this](bool is_checked) {
            if (m_light_device) {
              m_light_device->set_state(is_checked);
            }
          });

  connect(btn_reduce_brightness.data(), &QPushButton::pressed, this, [this]() {
    if (m_brightness_slider) {
      const int value = m_brightness_slider->sliderPosition();
      m_brightness_slider->setValue(value -
                                    m_brightness_slider->tickInterval());
    }
  });

  connect(btn_increase_brightness.data(), &QPushButton::pressed, this,
          [this]() {
            if (m_brightness_slider) {
              const int value = m_brightness_slider->sliderPosition();
              m_brightness_slider->setValue(
                  value + m_brightness_slider->tickInterval());
            }
          });

  connect(m_light_device.data(), &SmartLight::brightness_changed, this,
          [this](int value) {
            if (m_brightness_slider.isNull() ||
                m_brightness_slider->isSliderDown()) {
              return;
            }
            const QSignalBlocker blocker(m_brightness_slider.data());
            m_brightness_slider->setValue(value);
            this->update_slider_label_pos();
          });

  connect(m_light_device.data(), &SmartLight::state_changed, this,
          [this](bool is_checked) {
            if (m_toggle_button.isNull()) {
              return;
            }
            const QSignalBlocker blocker(m_toggle_button.data());
            m_toggle_button->setChecked(is_checked);
            m_toggle_button->setText(is_checked ? "ON" : "OFF");
          });

  connect(btn_settings.data(), &QPushButton::clicked, this, [this]() {
    if (m_light_device.isNull()) {
      return;
    }
    auto *dialog =                                                    // NOLINT
        new SmartLightSettingsDialog(m_light_device, this->window()); // NOLINT

    connect(dialog, &QDialog::finished, this, [this](int result) {
      if (result == QDialog::Accepted) {
        // Update logic goes here
      }
    });

    dialog->open();
  });
}

#pragma once
#include "backdrop_dialog.h"
#include "smart_light.h"
#include "utils.h"

#include <QButtonGroup>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QVBoxLayout>

#include <QTimer>

enum class ColorTempSetting : std::uint8_t { Cool = 0, Neutral = 1, Warm = 2 };

struct TempSetting {
  QString label;
  ColorTempSetting level;
  int temp;
};

class SmartLightSettingsDialog : public BackdropDialog {
public:
  explicit SmartLightSettingsDialog(const QPointer<SmartLight> &light,
                                    QWidget *parent = nullptr)
      : BackdropDialog(parent), light(light) {
    setup_ui();
  };

protected:
  void setup_ui() override {
    if (light.isNull()) {
      return;
    }

    auto *layout = new QVBoxLayout(this); // NOLINT
    layout->addWidget(new QLabel(         // NOLINT
        QString("Light Configuration for %1").arg(light->display_name()),
        this));

    auto min_temp = light->get_color_temp_range().min;
    auto max_temp = light->get_color_temp_range().max;
    auto neutral_temp = light->get_color_temp_range().neutral;

    auto *button_group_box = new QGroupBox("Set Color Temp", this);
    auto *button_group_layout = new QHBoxLayout(button_group_box); // NOLINT

    auto *buttonGroup = new QButtonGroup(this); // NOLINT
    buttonGroup->setExclusive(true);

    const auto options = {TempSetting{.label = "Cool",
                                      .level = ColorTempSetting::Cool,
                                      .temp = min_temp},
                          TempSetting{.label = "Neutral",
                                      .level = ColorTempSetting::Neutral,
                                      .temp = neutral_temp},
                          TempSetting{.label = "Warm",
                                      .level = ColorTempSetting::Warm,
                                      .temp = max_temp}};

    for (const auto &option : options) {
      auto *button = new QPushButton(option.label, button_group_box); // NOLINT
      button->setCheckable(true);

      button->setChecked(light->color_temp() == option.temp);

      button_group_layout->addWidget(button);
      buttonGroup->addButton(button, static_cast<int>(option.level));
    }

    layout->addWidget(button_group_box);

    auto *closeBtn = new QPushButton("Close", this);
    layout->addWidget(closeBtn);

    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(buttonGroup, &QButtonGroup::idClicked, [this](int choice) {
      if (light.isNull()) {
        return;
      }

      const auto selected = static_cast<ColorTempSetting>(choice);
      const ColorTempRange range = light->get_color_temp_range();

      switch (selected) {
      case ColorTempSetting::Cool:
        light->set_color_temp(range.min);
        break;
      case ColorTempSetting::Neutral:
        light->set_color_temp(range.neutral);
        break;
      case ColorTempSetting::Warm:
        light->set_color_temp(range.max);
        break;
      }
    });
  }

  void resizeEvent(QResizeEvent *event) override {
    QDialog::resizeEvent(event);

    // Defer centering to the next event loop tick to ensure
    // QSS-defined padding and font-metrics are fully applied.
    QTimer::singleShot(0, this,
                       [this]() { WidgetUtils::center_in_window(this); });
  }

private:
  QPointer<SmartLight> light;
};
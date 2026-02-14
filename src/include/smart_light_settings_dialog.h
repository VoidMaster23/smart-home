#pragma once
#include "backdrop_dialog.h"
#include "smart_light.h"
#include "utils.h"

#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

#include <QTimer>

class SmartLightSettingsDialog : public BackdropDialog {
public:
  explicit SmartLightSettingsDialog(SmartLight *light,
                                    QWidget *parent = nullptr)
      : BackdropDialog(parent), light(light) {
    setup_ui();
  };

protected:
  void setup_ui() override {
    auto *layout = new QVBoxLayout(this); // NOLINT
    layout->addWidget(new QLabel(
        QString("Light Configuration for %1").arg(light->display_name()),
        this)); // NOLINT

    auto *closeBtn = new QPushButton("Close", this);
    layout->addWidget(closeBtn);

    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
  }

  void resizeEvent(QResizeEvent *event) override {
    QDialog::resizeEvent(event);

    // Defer centering to the next event loop tick to ensure
    // QSS-defined padding and font-metrics are fully applied.
    QTimer::singleShot(0, this,
                       [this]() { WidgetUtils::center_in_window(this); });
  }

private:
  SmartLight *light;
};
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

// TODO: pass the smart light here
// TODO: make a base class for this that has the backdrop logic baked in, for reusability later

class SmartLightSettingsDialog : public QDialog {
public:
  explicit SmartLightSettingsDialog(QWidget *parent = nullptr)
      : QDialog(parent) {

    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setModal(true);

    auto *layout = new QVBoxLayout(this);                       // NOLINT
    layout->addWidget(new QLabel("Light Configuration", this)); // NOLINT

    auto *closeBtn = new QPushButton("Close", this);
    layout->addWidget(closeBtn);

    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    setAttribute(Qt::WA_DeleteOnClose);
  }
};
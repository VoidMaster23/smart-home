#pragma once
#include <QDialog>
#include <QWidget>
#include <QObject>
#include <QEvent>


class BackdropDialog : public QDialog {
    Q_OBJECT
    public: 
        explicit BackdropDialog(QWidget *parent): QDialog(parent) {
            backdrop = new QWidget(parent->window()); //NOLINT

            backdrop->setObjectName("modalBackdrop");
            backdrop->setGeometry(parent->window()->rect());
            backdrop->show();

            
            setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
            this->show();
            this->raise();
            setAttribute(Qt::WA_DeleteOnClose);

            connect(this, &QDialog::finished, backdrop, &QWidget::deleteLater);
            backdrop->installEventFilter(this);

        }
    
    protected: 
        virtual void setup_ui() = 0;
    
    private: 
        QWidget *backdrop;
        bool eventFilter(QObject *object, QEvent *event) override {
            if(object == backdrop && event->type() == QEvent::MouseButtonPress) {
                this->reject();
                return true;
            }
            return QDialog::eventFilter(object, event);
        }

};
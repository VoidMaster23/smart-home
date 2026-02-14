#pragma once

#include <QString>
#include <QWidget>

namespace StringUtils {

QString format_for_display(const QString &string);

}

namespace WidgetUtils {
void center_in_window(QWidget *widget);
}
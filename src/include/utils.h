#pragma once

#include <QString>
#include <QWidget>
#include <QPointer>

namespace StringUtils {

QString format_for_display(const QString &string);

}

namespace WidgetUtils {
void center_in_window(const QPointer<QWidget> &widget);
}
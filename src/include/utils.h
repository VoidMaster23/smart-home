#pragma once

#include <QStringView>
#include <QWidget>
#include <QPointer>

namespace StringUtils {

QString format_for_display(QStringView string);

}

namespace WidgetUtils {
void center_in_window(const QPointer<QWidget> &widget);
}

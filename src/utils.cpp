#include "utils.h"
#include <QStringView>
#include <QStringList>
#include <QWidget>

#include <QGuiApplication>
#include <QPointer>
#include <QScreen>

QString StringUtils::format_for_display(QStringView string) {
  const auto words_list{string.split('_', Qt::SkipEmptyParts)};
  QStringList formatted;
  for (const auto &word : words_list) {
    formatted << word.at(0).toUpper() + word.mid(1).toString();
  }

  return formatted.join(' ');
}

void WidgetUtils::center_in_window(const QPointer<QWidget> &widget) {
  if (widget.isNull()) {
    return;
  }

  auto *parent = widget->parentWidget();
  if (parent == nullptr) {
    return;
  }

  widget->ensurePolished();
  widget->adjustSize();

  // Use the content rectangle to avoid title-bar confusion
  const QRect parent_area = parent->rect();
  const QRect child_rect = widget->rect();

  // The math that never fails if dimensions are correct
  const int x = (parent_area.width() - child_rect.width()) / 2;
  const int y = (parent_area.height() - child_rect.height()) / 2;

  widget->move(x, y);
}

#include "utils.h"
#include <QString>
#include <QStringList>
#include <QWidget>

#include <QGuiApplication>
#include <QScreen>

QString StringUtils::format_for_display(const QString &string) {
  QStringList words_list{string.split('_', Qt::SkipEmptyParts)};

  for (auto &word : words_list) {
    word = word.at(0).toUpper() + word.mid(1);
  }

  return words_list.join(' ');
}

void WidgetUtils::center_in_window(QWidget *widget) {
  widget->ensurePolished();
  widget->adjustSize();

  // Use the content rectangle to avoid title-bar confusion
  QRect parentArea = widget->parentWidget()->rect();
  QRect childRect = widget->rect();

  // The math that never fails if dimensions are correct
  int x = (parentArea.width() - childRect.width()) / 2;
  int y = (parentArea.height() - childRect.height()) / 2;

  widget->move(x, y);
}
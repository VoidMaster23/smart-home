#include "utils.h"
#include <QFont>
#include <QString>
#include <QStringList>

QString StringUtils::format_for_display(const QString &string) {
  QStringList words_list{string.split('_', Qt::SkipEmptyParts)};

  for (auto &word : words_list) {
    word = word.at(0).toUpper() + word.mid(1);
  }

  return words_list.join(' ');
}
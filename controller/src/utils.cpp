#include "utils.h"
#include <QStringView>
#include <QStringList>
#include <algorithm>
#include <ranges>

[[nodiscard]] QString StringUtils::format_for_display(QStringView string) {
  const auto words_list{string.split('_', Qt::SkipEmptyParts)};
  QStringList formatted;
  std::ranges::transform(words_list, std::back_inserter(formatted), [](const auto &word) {
    return word.at(0).toUpper() + word.mid(1).toString();
  });

  return formatted.join(' ');
}

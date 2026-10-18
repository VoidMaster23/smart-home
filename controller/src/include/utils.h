#pragma once

#include <QStringView>
#include <QString>

namespace StringUtils {

[[nodiscard]] QString format_for_display(QStringView string);

}

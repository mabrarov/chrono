//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ma/chrono/currencynumericcodedelegate.h>

namespace ma {
namespace chrono {

CurrencyNumericCodeDelegate::CurrencyNumericCodeDelegate(QObject* parent)
  : QStyledItemDelegate(parent)
{
}

CurrencyNumericCodeDelegate::~CurrencyNumericCodeDelegate()
{
}

QString CurrencyNumericCodeDelegate::displayText(
    const QVariant& value, const QLocale& locale) const
{
  if (value.canConvert<int>())
  {
    return QString::fromLatin1("%1").arg(
        value.value<int>(), 3, 10, QLatin1Char('0'));
  }
  return QStyledItemDelegate::displayText(value, locale);
}

} // namespace chrono
} // namespace ma

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ma/chrono/model/currencyamount.h>
#include <ma/chrono/amountdelegate.h>

namespace ma {
namespace chrono {

AmountDelegate::AmountDelegate(QObject* parent)
  : QStyledItemDelegate(parent)
{
}

AmountDelegate::~AmountDelegate()
{
}

QString AmountDelegate::displayText(const QVariant& value, 
    const QLocale& locale) const
{
  if (value.canConvert<model::CurrencyAmount>())
  {
    return value.value<model::CurrencyAmount>().toString();
  }
  return QStyledItemDelegate::displayText(value, locale);
}

} // namespace chrono
} // namespace ma

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ma/chrono/model/rateamount.h>
#include <ma/chrono/ratedelegate.h>

namespace ma {
namespace chrono {

RateDelegate::RateDelegate(QObject* parent)
  : QStyledItemDelegate(parent)
{
}

RateDelegate::~RateDelegate()
{
}

QString RateDelegate::displayText(
    const QVariant& value, const QLocale& locale) const
{
  if (value.canConvert<model::RateAmount>())
  {
    return value.value<model::RateAmount>().toString();
  }
  return QStyledItemDelegate::displayText(value, locale);
}

} // namespace chrono
} // namespace ma

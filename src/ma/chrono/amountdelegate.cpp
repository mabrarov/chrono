//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
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
} //namespace ma

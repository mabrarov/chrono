//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <ma/chrono/currencynumericcodedelegate.h>

namespace ma
{
  namespace chrono
  {
    CurrencyNumericCodeDelegate::CurrencyNumericCodeDelegate(QObject* parent)
      : QStyledItemDelegate(parent)
    {
    }

    CurrencyNumericCodeDelegate::~CurrencyNumericCodeDelegate()
    {
    }

    QString CurrencyNumericCodeDelegate::displayText(const QVariant& value, const QLocale& locale) const
    {
      if (value.canConvert<int>())
      {
        return QString("%1").arg(value.value<int>(), 3, 10, QLatin1Char('0'));
      }
      return QStyledItemDelegate::displayText(value, locale);
    }

  } // namespace chrono
} //namespace ma
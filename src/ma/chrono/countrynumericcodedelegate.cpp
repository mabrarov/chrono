//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <ma/chrono/countrynumericcodedelegate.h>

namespace ma
{
  namespace chrono
  {
    CountryNumericCodeDelegate::CountryNumericCodeDelegate(QObject* parent)
      : QStyledItemDelegate(parent)
    {
    }

    CountryNumericCodeDelegate::~CountryNumericCodeDelegate()
    {
    }

    QString CountryNumericCodeDelegate::displayText(const QVariant& value, const QLocale& locale) const
    {
      if (value.canConvert<int>())
      {
        return QString("%1").arg(value.value<int>(), 3, 10, QLatin1Char('0'));
      }
      return QStyledItemDelegate::displayText(value, locale);
    }

  } // namespace chrono
} //namespace ma
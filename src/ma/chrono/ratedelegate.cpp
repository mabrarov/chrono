//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <ma/chrono/model/rateamount.h>
#include <ma/chrono/ratedelegate.h>

namespace ma
{
  namespace chrono
  {
    RateDelegate::RateDelegate(QObject* parent)
      : QStyledItemDelegate(parent)
    {
    }

    RateDelegate::~RateDelegate()
    {
    }

    QString RateDelegate::displayText(const QVariant& value, const QLocale& locale) const
    {
      if (value.canConvert<model::RateAmount>())
      {
        return value.value<model::RateAmount>().toString();
      }
      return QStyledItemDelegate::displayText(value, locale);
    }

  } // namespace chrono
} //namespace ma
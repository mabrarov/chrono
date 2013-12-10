//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_CURRENCYNUMERICCODEDELEGATE_H
#define MA_CHRONO_CURRENCYNUMERICCODEDELEGATE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <QStyledItemDelegate>

namespace ma {
namespace chrono {

class CurrencyNumericCodeDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public: 
  explicit CurrencyNumericCodeDelegate(QObject* parent = 0);
  ~CurrencyNumericCodeDelegate();

  QString displayText(const QVariant& value, const QLocale& locale) const;
}; // class CurrencyNumericCodeDelegate

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_CURRENCYNUMERICCODEDELEGATE_H

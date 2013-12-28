//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_RATEDELEGATE_H
#define MA_CHRONO_RATEDELEGATE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <QStyledItemDelegate>

namespace ma {
namespace chrono {

class RateDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public: 
  explicit RateDelegate(QObject* parent = 0);
  ~RateDelegate();

  QString displayText(const QVariant& value, const QLocale& locale) const;
}; // class RateDelegate

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_RATEDELEGATE_H

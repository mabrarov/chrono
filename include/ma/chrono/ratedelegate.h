//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
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
} // namespace ma

#endif // MA_CHRONO_RATEDELEGATE_H

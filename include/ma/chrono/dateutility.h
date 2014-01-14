//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_DATEUTILITY_H
#define MA_CHRONO_DATEUTILITY_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/utility.hpp>
#include <qglobal.h>

QT_BEGIN_NAMESPACE
class QDate;
QT_END_NAMESPACE

namespace ma {
namespace chrono {    

class DateUtility : private boost::noncopyable
{
public:
  static QDate getWeeekStart(const QDate& date);
  static QDate getWeeekEnd(const QDate& date);
  static QDate getMonthStart(const QDate& date);
  static QDate getMonthEnd(const QDate& date);

private:
  DateUtility(); 
}; // class ModelUtility

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_DATEUTILITY_H

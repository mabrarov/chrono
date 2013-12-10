//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_DATEUTILITY_H
#define MA_CHRONO_DATEUTILITY_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/utility.hpp>
#include <QDate>

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
} //namespace ma

#endif // MA_CHRONO_DATEUTILITY_H

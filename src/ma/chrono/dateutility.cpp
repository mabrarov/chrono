//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <QDate>
#include <ma/chrono/dateutility.h>

namespace ma {
namespace chrono {

QDate DateUtility::getWeeekStart(const QDate& date)
{      
  int dow = date.dayOfWeek();
  if (int shift = dow - Qt::Monday)
  {
    return date.addDays(-shift);
  }
  return date;
}

QDate DateUtility::getWeeekEnd(const QDate& date)
{      
  int dow = date.dayOfWeek();
  if (int shift = Qt::Sunday - dow)
  {
    return date.addDays(shift);
  }
  return date;
}

QDate DateUtility::getMonthStart(const QDate& date)
{
  int dom = date.day();
  if (int shift = dom - 1)
  {
    return date.addDays(-shift);
  }
  return date;
}

QDate DateUtility::getMonthEnd(const QDate& date)
{
  int dom = date.day();
  int dim = date.daysInMonth();
  if (int shift = dim - dom)
  {
    return date.addDays(shift);
  }
  return date;
}

} // namespace chrono
} // namespace ma
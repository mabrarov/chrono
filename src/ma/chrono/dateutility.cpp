//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <ma/chrono/dateutility.h>

namespace ma
{
  namespace chrono
  {        
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
} //namespace ma
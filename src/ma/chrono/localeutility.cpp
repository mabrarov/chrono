//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <QDateEdit>
#include <QCalendarWidget>
#include <ma/chrono/localeutility.h>

namespace ma {
namespace chrono {

void LocaleUtility::setupLocaleAwareWidget(QCalendarWidget* calendar)
{
  calendar->setFirstDayOfWeek(Qt::Monday);
  calendar->setGridVisible(true); 
}

void LocaleUtility::setupLocaleAwareWidget(QDateEdit* dateEdit)
{      
  dateEdit->setDate(QDate::currentDate());
  if (QCalendarWidget* calendar = dateEdit->calendarWidget())
  {
    setupLocaleAwareWidget(calendar);        
  }
}
    
} // namespace chrono
} // namespace ma

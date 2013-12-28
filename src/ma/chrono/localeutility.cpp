//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <QDateEdit>
#include <QCalendarWidget>
#include <ma/chrono/localeutility.h>

namespace ma
{
  namespace chrono
  {        
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
} //namespace ma

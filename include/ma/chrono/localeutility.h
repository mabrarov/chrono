//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_LOCALEUTILITY_H
#define MA_CHRONO_LOCALEUTILITY_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/utility.hpp>

QT_BEGIN_NAMESPACE  
class QCalendarWidget;
class QDateEdit;
QT_END_NAMESPACE

namespace ma {
namespace chrono {    

class LocaleUtility : private boost::noncopyable
{
public:
  static void setupLocaleAwareWidget(QCalendarWidget* calendar);
  static void setupLocaleAwareWidget(QDateEdit* dateEdit);      

private:
  LocaleUtility(); 
}; // class ModelUtility

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_LOCALEUTILITY_H

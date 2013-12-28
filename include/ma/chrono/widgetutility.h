//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_WIDGETUTILITY_H
#define MA_CHRONO_WIDGETUTILITY_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/noncopyable.hpp>
#include <ma/chrono/types.h>

namespace ma {
namespace chrono {

class WidgetUtility : private boost::noncopyable
{
public:
  static void setReadOnly(QWidget& widget, bool readOnly);
  static void setReadOnly(const QWidgetList& widgets, bool readOnly);
  static void show(const QWidgetList& widgets, bool visible = true);

private:
  WidgetUtility(); 
}; // class WidgetUtility

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_WIDGETUTILITY_H

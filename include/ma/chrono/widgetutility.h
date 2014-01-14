//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
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
} // namespace ma

#endif // MA_CHRONO_WIDGETUTILITY_H

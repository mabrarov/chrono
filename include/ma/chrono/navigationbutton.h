//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_NAVIGATIONBUTTON_H
#define MA_CHRONO_NAVIGATIONBUTTON_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <qglobal.h>
#include <QToolButton>
#include <ma/chrono/navigationbutton_fwd.h>

QT_BEGIN_NAMESPACE
class QStyleOption;
class QPainter;  
QT_END_NAMESPACE

namespace ma {
namespace chrono {

class NavigationButton : public QToolButton
{
  Q_OBJECT

public:
  explicit NavigationButton(QWidget* parent = 0);
  ~NavigationButton();

protected:
  void paintEvent(QPaintEvent* event);

private:
  Q_DISABLE_COPY(NavigationButton)
  void drawBackground(QStyleOption& option, QPainter& painter) const;
  void drawPressedBackground(QStyleOption& option, QPainter& painter) const;
  void drawHotBackground(QStyleOption& option, QPainter& painter) const;
  void drawHotCheckedBackground(QStyleOption& option, QPainter& painter) const;
  void drawDisabledBackground(QStyleOption& option, QPainter& painter) const;
  void drawCheckedBackground(QStyleOption& option, QPainter& painter) const;
  void drawNormalBackground(QStyleOption& option, QPainter& painter) const;      
}; // class NavigationButton 

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_NAVIGATIONBUTTON_H

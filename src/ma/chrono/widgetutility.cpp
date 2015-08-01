//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <algorithm>
#include <functional>
#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QAbstractSpinBox>
#include <ma/chrono/widgetutility.h>

namespace ma {
namespace chrono {

namespace {

void setWidgetReadOnly(QWidget* widget, bool readOnly)
{  
  if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget))
  {
    lineEdit->setReadOnly(readOnly);
  }
  else if (QTextEdit* textEdit = qobject_cast<QTextEdit*>(widget))
  {
    textEdit->setReadOnly(readOnly);
  }
  else if (QAbstractSpinBox* spinBox = qobject_cast<QAbstractSpinBox*>(widget))
  {
    spinBox->setReadOnly(readOnly);
  }
  else
  {
    widget->setEnabled(!readOnly);
  }
}

} // anonymous namespace

void WidgetUtility::setReadOnly(QWidget& widget, bool readOnly)
{  
  setWidgetReadOnly(&widget, readOnly);
}

void WidgetUtility::setReadOnly(const QWidgetList& widgets, bool readOnly)
{
  std::for_each(widgets.begin(), widgets.end(), 
      std::bind2nd(std::ptr_fun(&setWidgetReadOnly), readOnly));
}

void WidgetUtility::show(const QWidgetList& widgets, bool visible)
{
  std::for_each(widgets.begin(), widgets.end(), 
      std::bind2nd(std::mem_fun(&QWidget::setVisible), visible));
}

} // namespace chrono
} // namespace ma

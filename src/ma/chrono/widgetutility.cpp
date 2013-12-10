//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <algorithm>
#include <functional>
#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <ma/chrono/widgetutility.h>

namespace ma {
namespace chrono {

namespace {

void setWidgetReadOnly(QWidget* widget, bool readOnly)
{  
  if (QLineEdit* edit = qobject_cast<QLineEdit*>(widget))
  {
    edit->setReadOnly(readOnly);
  }
  else if (QTextEdit* textEdit = qobject_cast<QTextEdit*>(widget))
  {
    textEdit->setReadOnly(readOnly);
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
} //namespace ma

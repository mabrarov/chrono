/*
TRANSLATOR ma::chrono::CommandBar 
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <memory>
#include <QSizePolicy>
#include <QPaintEvent>
#include <QPainter>
#include <QFont>
#include <QLinearGradient>
#include <QPalette>
#include <QAction>
#include <QIcon>
#include <QStyleOption>
#include <QHBoxLayout>
#include <QToolButton>
#include <ma/chrono/types.h>
#include <ma/chrono/constants.h>
#include <ma/chrono/commandbutton.h>
#include <ma/chrono/commandbar.h>

namespace ma {
namespace chrono {

CommandBar::CommandBar(QWidget* parent)
  : QWidget(parent)      
  , mainLayout_(0)
  , trayLayout_(0)
{
  setAttribute(Qt::WA_NoSystemBackground);
  setAttribute(Qt::WA_OpaquePaintEvent);      
  int horMargin = 1 * logicalDpiX() / defaultDpiX;
  int verMargin = 1 * logicalDpiY() / defaultDpiY;      
  setContentsMargins(horMargin, verMargin, horMargin, verMargin);      

  QPalette palette(palette());
  palette.setColor(QPalette::ButtonText, Qt::white);      
  setPalette(palette); 

  QFont font(font());
  font.setPointSize(10);
  setFont(font);
     
  std::auto_ptr<QBoxLayout> leftLayout(new QHBoxLayout());                  
  leftLayout->setContentsMargins(0, 0, 0, 0);
  leftLayout->setSpacing(horMargin);

  std::auto_ptr<QBoxLayout> rightLayout(new QHBoxLayout());                  
  rightLayout->setContentsMargins(0, 0, 0, 0);
  rightLayout->setSpacing(horMargin);      
      
  QBoxLayout* mainLayout(new QHBoxLayout(this));
  mainLayout->setContentsMargins(0, 0, 0, 0);            
  mainLayout->setSpacing(0); 
  mainLayout->addLayout(leftLayout.get());      
  mainLayout->addStretch();
  mainLayout->addLayout(rightLayout.get());

  mainLayout_ = leftLayout.release();
  trayLayout_ = rightLayout.release();      
      
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);      
}
      
CommandBar::~CommandBar()
{
}

QSize CommandBar::minimumSizeHint() const
{
  QSize baseHint(QWidget::minimumSizeHint());
  return QSize(baseHint.width(),
      qMax(baseHint.height(), 
          minimumSizeHeight * logicalDpiY() / defaultDpiY));
} 

void CommandBar::updateTabOrder()
{
  QWidget* lastLeft = updateTabOrder(mainLayout_);
  updateTabOrder(trayLayout_, lastLeft);
  QWidget* focusProxyWidget = findFocusProxy(mainLayout_);
  if (!focusProxyWidget)
  {
    focusProxyWidget = findFocusProxy(trayLayout_);
  }
  setFocusProxy(focusProxyWidget);
}

void CommandBar::addCommand(QBoxLayout* layout, QAction* action, bool showText)
{
  std::auto_ptr<QToolButton> button(new CommandButton(this));
  button->setDefaultAction(action);
  if (action->menu())
  {
    button->setPopupMode(QToolButton::InstantPopup);
  }      
  if (showText)
  {
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  }
  else
  {
    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
  }
  QObject::connect(action, SIGNAL(changed()), button.get(), SLOT(updateState()));
  layout->addWidget(button.release());      
}
    
QWidget* CommandBar::updateTabOrder(QLayout* layout, QWidget* first)
{
  int itemCount = layout->count();
  for (int i = 0; i != itemCount; ++i)
  {
    QWidget* widget = layout->itemAt(i)->widget();
    if (widget)
    {
      if (first)
      {
        QWidget::setTabOrder(first, widget);
      }
      first = widget;
    }        
  }
  return first;
}
   
void CommandBar::addCommand(QAction* action, CommandArea area, bool showText)
{          
  if (main == area)
  {        
    addCommand(mainLayout_, action, showText);        
  }
  else
  {                
    addCommand(trayLayout_, action, showText);        
  }
  updateTabOrder();
}

void CommandBar::addCommands(const QActionList& actions, CommandArea area, bool showText)
{
  typedef QActionList action_list;
  typedef action_list::const_iterator action_iterator;

  QBoxLayout* layout = main == area ? mainLayout_ : trayLayout_;
  for (action_iterator it = actions.begin(), end = actions.end(); it != end; ++it)      
  {
    addCommand(layout, *it, showText);        
  }
  updateTabOrder();
}

void CommandBar::removeCommands(QAction* action)
{
  removeCommands(mainLayout_, action);
  removeCommands(trayLayout_, action);
  updateTabOrder();
}

void CommandBar::removeCommands(QAction* action, CommandArea area)
{
  if (main == area)
  {
    removeCommands(mainLayout_, action);
  }
  else
  {
    removeCommands(trayLayout_, action);
  }    
  updateTabOrder();
}

void CommandBar::removeCommands(const QActionList& actions)
{
  typedef QActionList action_list;
  typedef action_list::const_iterator action_iterator;
      
  for (action_iterator it = actions.begin(), end = actions.end(); it != end; ++it)      
  {
    removeCommands(mainLayout_, *it);
    removeCommands(trayLayout_, *it);
  }
  updateTabOrder();
}

bool CommandBar::hasCommand(QAction* action) const
{
  return hasCommand(mainLayout_, action) || hasCommand(trayLayout_, action);
}

bool CommandBar::hasCommand(QAction* action, CommandArea area) const
{
  if (main == area)
  {
    return hasCommand(mainLayout_, action);
  }
  else 
  {
    return hasCommand(trayLayout_, action);
  }
}

CommandButton* CommandBar::hasCommand(QLayout* layout, QAction* action)
{      
  int itemCount = layout->count();
  for (int i = 0; i != itemCount; ++i)
  {
    QWidget* widget = layout->itemAt(i)->widget();
    if (widget)
    {
      CommandButton* button = qobject_cast<CommandButton*>(widget);
      if (button)
      {
        if (button->defaultAction() == action)
        {
          return button;
        }            
      }                   
    }        
  }      
  return 0;
}    

void CommandBar::removeCommands(QLayout* layout, QAction* action)
{      
  for (int i = 0; i != layout->count(); )
  {
    bool deleted = false;
    QWidget* widget = layout->itemAt(i)->widget();
    if (widget)
    {
      CommandButton* button = qobject_cast<CommandButton*>(widget);
      if (button)
      {
        if (button->defaultAction() == action)
        {
          layout->removeWidget(button);
          deleted = true;
          delete button;
        }            
      }
    }
    if (!deleted)
    {
      ++i;
    }
  } 
  updateTabOrder(layout);
}

QWidget* CommandBar::findFocusProxy(QLayout* layout)
{
  QWidget* firstFocusableWidget = 0;      
  int itemCount = layout->count();
  for (int i = 0; i != itemCount; ++i)
  {
    QWidget* widget = layout->itemAt(i)->widget();
    if (widget)
    {
      CommandButton* button = qobject_cast<CommandButton*>(widget);
      if (button)
      {
        return button;
      }
      if (!firstFocusableWidget)
      {
        if (Qt::TabFocus & widget->focusPolicy())
        {
          firstFocusableWidget = widget;
        }
      }
    }        
  }
  return firstFocusableWidget;
}

void CommandBar::paintEvent(QPaintEvent* /*event*/)
{
  QPainter painter(this);
      
  int topHeight = (height() + 1) / 2;
  QRect topRect(0, 0, width(), topHeight);
  QLinearGradient topFade(0, topRect.top(), 0, topRect.bottom());
  topFade.setColorAt(0, QColor(75, 95, 148));
  topFade.setColorAt(1, QColor(88, 111, 169));
  painter.fillRect(topRect, topFade);      

  int realBorderWidth  = borderWidth * logicalDpiX() / defaultDpiX;      
  int realBorderHeight = borderHeight * logicalDpiY() / defaultDpiY; 

  QRect bottomRect(0, topHeight, width(), height() - realBorderHeight - topHeight);
  QLinearGradient bottomFade(0, bottomRect.top(), 0, bottomRect.bottom());
  bottomFade.setColorAt(0, QColor(77, 102, 165));
  bottomFade.setColorAt(1, QColor(106, 130, 194));
  painter.fillRect(bottomRect, bottomFade);
      
  QRect topBorderRect(realBorderWidth, 0, width() - 2 * realBorderWidth, realBorderHeight);      
  painter.fillRect(topBorderRect, QColor(113, 129, 170));

  QRect bottomBorderRect(0, height() - realBorderHeight, width(), realBorderHeight);
  painter.fillRect(bottomBorderRect, QColor(86, 108, 158));

  QRect bottomBorderRect2(realBorderWidth, height() - 2 * realBorderHeight, width() - 2 * realBorderWidth, realBorderHeight);      
  painter.fillRect(bottomBorderRect2, QColor(141, 160, 210));      
}    

} // namespace chrono
} // namespace ma

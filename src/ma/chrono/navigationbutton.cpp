/*
TRANSLATOR ma::chrono::NavigationButton
*/

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <QStyle>
#include <QPainter>
#include <QStyleOptionToolButton>
#include <QLinearGradient>
#include <ma/chrono/constants.h>
#include <ma/chrono/navigationbutton.h>

namespace ma {
namespace chrono {

NavigationButton::NavigationButton(QWidget* parent)
  : QToolButton(parent)      
{
  setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
  setAutoRaise(true);
}

NavigationButton::~NavigationButton()
{
}

void NavigationButton::paintEvent(QPaintEvent* /*event*/)
{
  QPainter painter(this);
  QStyleOptionToolButton buttonOption;
  initStyleOption(&buttonOption);      

  QStyle::State flags = buttonOption.state;            
  if (testAttribute(Qt::WA_UnderMouse) && isActiveWindow())
  {
    flags |= QStyle::State_MouseOver;
  }

  QStyle* style(style());      
  QRect buttonRect = style->subControlRect(
      QStyle::CC_ToolButton, &buttonOption, QStyle::SC_ToolButton, this);

  QStyle::State bflags = buttonOption.state & ~QStyle::State_Sunken;      
  if (!(bflags & QStyle::State_MouseOver) || !(bflags & QStyle::State_Enabled))
  {
    bflags &= ~QStyle::State_Raised;
  }
  if (buttonOption.state & QStyle::State_Sunken) 
  {
    if (buttonOption.activeSubControls & QStyle::SC_ToolButton) 
    {
      bflags |= QStyle::State_Sunken;          
    } 
    else if (buttonOption.activeSubControls & QStyle::SC_ToolButtonMenu) 
    {          
      bflags |= QStyle::State_MouseOver;
    }
  }   

  if (buttonOption.state & QStyle::State_HasFocus) 
  {
    bflags |= QStyle::State_Raised;        
  }

  QStyleOption backOption(0);
  backOption.palette = buttonOption.palette;
  if (buttonOption.subControls & QStyle::SC_ToolButton) 
  {
    if (flags & 
        (QStyle::State_Sunken | QStyle::State_On | QStyle::State_Raised))
    {          
      backOption.rect = buttonOption.rect;
      backOption.state = bflags;
      drawBackground(backOption, painter);
    }
  }
            
  QStyleOptionToolButton labelOption = buttonOption;
  labelOption.state = bflags & ~QStyle::State_Sunken;      
  int fw = 2 * logicalDpiX() / defaultDpiX;
  int fh = 2 * logicalDpiY() / defaultDpiY;      
  labelOption.rect = buttonRect.adjusted(fw, fh, -fw, -fh);      
  style->drawControl(QStyle::CE_ToolButtonLabel, &labelOption, &painter, this);
} // NavigationButton::paintEvent

void NavigationButton::drawBackground(
    QStyleOption& option, QPainter& painter) const
{ 
  //todo
  if (!(option.state & QStyle::State_Enabled))
  {
    drawDisabledBackground(option, painter);
  }
  else if (option.state & QStyle::State_Sunken)
  {        
    drawPressedBackground(option, painter);
  }
  else if (option.state & 
      QStyle::State_MouseOver || option.state & QStyle::State_HasFocus)
  {
    if (option.state & QStyle::State_On)
    {
      drawHotCheckedBackground(option, painter);
    }
    else
    {
      drawHotBackground(option, painter);
    }
  }
  else if (option.state & QStyle::State_On)
  {
    drawCheckedBackground(option, painter);
  }
  else if (!(option.state & QStyle::State_AutoRaise))
  {
    drawHotBackground(option, painter);
  }
  else
  {
    drawNormalBackground(option, painter);
  }
} // NavigationButton::drawBackground

void NavigationButton::drawNormalBackground(
    QStyleOption& /*option*/, QPainter& /*painter*/) const
{
  // Do here nothing 
} // NavigationButton::drawNormalBackground

void NavigationButton::drawPressedBackground(
    QStyleOption& option, QPainter& painter) const
{
  //todo
  //style()->drawPrimitive(QStyle::PE_PanelButtonTool, &option, &painter, this);
  painter.save();

  const int horMargin = 1 * logicalDpiX() / defaultDpiX;
  const int verMargin = 1 * logicalDpiY() / defaultDpiY;
    
  QRect visibleRect = option.rect.adjusted(
      horMargin, verMargin, -horMargin, -verMargin);  
  QPen pen;
  pen.setWidth(1 * logicalDpiX() / defaultDpiX);
  static const QColor penColor(0, 0, 0, 120);
  pen.setColor(penColor);
  painter.setPen(pen);
  QLinearGradient fade1(visibleRect.left(), visibleRect.top(),
      visibleRect.left(), visibleRect.bottom());
  static const QColor fade1StartColor(0, 0, 0, 100);
  static const QColor fade1EndColor(0, 0, 0, 60);
  fade1.setColorAt(0, fade1StartColor);
  fade1.setColorAt(1, fade1EndColor);
  painter.setBrush(fade1);                
  painter.drawRect(visibleRect);
    
  painter.setPen(Qt::NoPen);
  QLinearGradient fade2(visibleRect.left(), visibleRect.top(),
      visibleRect.left(), visibleRect.bottom());
  static const QColor fade2StartColor(255, 255, 255, 5);
  static const QColor fade2MidColor(255, 255, 255, 100);
  static const QColor fade2EndColorQColor(255, 255, 255, 60);
  fade2.setColorAt(0, fade2StartColor);
  fade2.setColorAt(0.8, fade2MidColor);
  fade2.setColorAt(1, fade2EndColorQColor);
  painter.setBrush(fade2);                
  painter.drawRect(visibleRect);       

  painter.restore();
} // NavigationButton::drawPressedBackground

void NavigationButton::drawHotBackground(
    QStyleOption& option, QPainter& painter) const
{
  //todo
  //style()->drawPrimitive(QStyle::PE_PanelButtonTool, &option, &painter, this);
  painter.save();      

  const int horMargin = 1 * logicalDpiX() / defaultDpiX;
  const int verMargin = 1 * logicalDpiY() / defaultDpiY;
  
  QRect visibleRect = option.rect.adjusted(
      horMargin, verMargin, -horMargin, -verMargin);  
  QPen pen;
  pen.setWidth(1 * logicalDpiX() / defaultDpiX);
  static const QColor penColor(0, 0, 0, 80);
  pen.setColor(penColor);
  painter.setPen(pen);
  QLinearGradient fade1(visibleRect.left(), visibleRect.top(),
      visibleRect.left(), visibleRect.bottom());
  static const QColor fade1StartColor(0, 0, 0, 10);
  static const QColor fade1EndColor(0, 0, 0, 50);
  fade1.setColorAt(0, fade1StartColor);
  fade1.setColorAt(1, fade1EndColor);
  painter.setBrush(fade1);                
  painter.drawRect(visibleRect);
  
  painter.setPen(Qt::NoPen);
  QLinearGradient fade2(visibleRect.left(), visibleRect.top(),
      visibleRect.left(), visibleRect.bottom());
  static const QColor fade2StartColor(255, 255, 255, 90);
  static const QColor fade2MidColor(255, 255, 255, 140);
  static const QColor fade2EndColor(255, 255, 255, 0);
  fade2.setColorAt(0, fade2StartColor);
  fade2.setColorAt(0.5, fade2MidColor);
  fade2.setColorAt(1, fade2EndColor);
  painter.setBrush(fade2);                
  painter.drawRect(visibleRect);

  painter.restore();
} // NavigationButton::drawHotBackground

void NavigationButton::drawHotCheckedBackground(QStyleOption& option, QPainter& painter) const
{
  drawHotBackground(option, painter);
} // NavigationButton::drawHotCheckedBackground

void NavigationButton::drawDisabledBackground(QStyleOption& /*option*/, QPainter& /*painter*/) const
{
  //todo
  //style()->drawPrimitive(QStyle::PE_PanelButtonTool, &option, &painter, this);
} // NavigationButton::drawDisabledBackground

void NavigationButton::drawCheckedBackground(QStyleOption& option, QPainter& painter) const
{
  //todo
  //style()->drawPrimitive(QStyle::PE_PanelButtonTool, &option, &painter, this);
  drawPressedBackground(option, painter);
} // NavigationButton::drawCheckedBackground

} // namespace chrono
} // namespace ma

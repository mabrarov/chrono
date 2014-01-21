/*
TRANSLATOR ma::chrono::CommandButton
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <QStyle>
#include <QAction>
#include <QPainter>
#include <QStyleOptionToolButton>
#include <QLinearGradient>
#include <ma/chrono/constants.h>
#include <ma/chrono/commandbutton.h>

namespace ma {

const int additionalWidth  = 10;
const int additionalHeight = 4;

} // anonymous namespace

namespace ma {
namespace chrono {

CommandButton::CommandButton(QWidget* parent)
  : QToolButton(parent)      
{      
  setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
  setFocusPolicy(Qt::TabFocus);
  setAutoRaise(true);      
}

CommandButton::~CommandButton()
{
}

void CommandButton::updateState()
{            
  setVisible(defaultAction()->isVisible());
}

QSize CommandButton::minimumSizeHint() const
{
  QSize hint(QToolButton::minimumSizeHint());
  if (QToolButton::InstantPopup == popupMode())
  {
    hint.rwidth()  += additionalWidth  * logicalDpiX() / defaultDpiX;  
    hint.rheight() += additionalHeight * logicalDpiY() / defaultDpiY;
  }
  return hint;
}    

void CommandButton::paintEvent(QPaintEvent* /*event*/)
{
  QPainter painter(this);
  QStyleOptionToolButton buttonOption;
  initStyleOption(&buttonOption);      

  QStyle* style(style());      
  QRect buttonRect = style->subControlRect(
      QStyle::CC_ToolButton, &buttonOption, QStyle::SC_ToolButton, this);

  QStyle::State flags = buttonOption.state;
  if (testAttribute(Qt::WA_UnderMouse) && isActiveWindow())
  {
    flags |= QStyle::State_MouseOver;
  }      
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

  // Draw background
  QStyleOption backOption(0);
  backOption.palette = buttonOption.palette;
  if (buttonOption.subControls & QStyle::SC_ToolButton)
  {
    if (flags & (QStyle::State_Sunken | QStyle::State_On | QStyle::State_Raised)) 
    {          
      backOption.rect = buttonOption.rect;
      backOption.state = bflags;
      drawBackground(backOption, painter);
    }
  }
      
  // Draw label
  QStyleOptionToolButton labelOption = buttonOption;
  labelOption.state = bflags & ~QStyle::State_Sunken;      
  int fw = 2 * logicalDpiX() / defaultDpiX;
  int fh = 2 * logicalDpiY() / defaultDpiY;
  labelOption.rect = buttonRect.adjusted(fw, fh, -fw, -fh);
  if (buttonOption.features & QStyleOptionToolButton::HasMenu)
  {
    labelOption.rect.adjust(0, 0, -additionalWidth, 0);
  }
  style->drawControl(QStyle::CE_ToolButtonLabel, &labelOption, &painter, this);

  if (buttonOption.subControls & QStyle::SC_ToolButtonMenu || 
    buttonOption.features & QStyleOptionToolButton::HasMenu)
  {                 
    QStyleOptionToolButton mbiOption = buttonOption;
    mbiOption.rect.setLeft(labelOption.rect.right() + 1);
    mbiOption.rect.setTop(
        labelOption.rect.top() + 2 * logicalDpiY() / defaultDpiY);
    mbiOption.rect.setWidth(additionalWidth - 3);
    mbiOption.rect.setBottom(labelOption.rect.bottom());        
    mbiOption.state &= ~QStyle::State_Sunken;        
    style->drawPrimitive(
        QStyle::PE_IndicatorArrowDown, &mbiOption, &painter, this);
  }
} // CommandButton::paintEvent

void CommandButton::drawBackground(
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
  else if (option.state & QStyle::State_MouseOver 
      || option.state & QStyle::State_HasFocus)
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
} // CommandButton::drawBackground

void CommandButton::drawNormalBackground(
    QStyleOption& /*buttonOption*/, QPainter& /*painter*/) const
{
  // Do here nothing 
} // CommandButton::drawNormalBackground

void CommandButton::drawPressedBackground(
    QStyleOption& buttonOption, QPainter& painter) const
{
  //todo
  //style()->drawPrimitive(
  //    QStyle::PE_PanelButtonTool, &buttonOption, &painter, this);
  painter.save();

  const int horMargin = 1 * logicalDpiX() / defaultDpiX;
  const int verMargin = 2 * logicalDpiY() / defaultDpiY;
  
  const QRect visibleRect = buttonOption.rect.adjusted(
      horMargin, verMargin, -horMargin, -verMargin);
  QPen pen;
  pen.setWidth(1 * logicalDpiX() / defaultDpiX);
  static const QColor penColor(0, 0, 0, 230);
  pen.setColor(penColor);
  painter.setPen(pen);
  QLinearGradient fade1(visibleRect.left(), visibleRect.top(), 
      visibleRect.left(), visibleRect.bottom());
  static const QColor fade1StartColor(0, 0, 0, 15);
  static const QColor fade1EndColor(0, 0, 0, 40);
  fade1.setColorAt(0, fade1StartColor);
  fade1.setColorAt(1, fade1EndColor);
  painter.setBrush(fade1);                
  painter.drawRect(visibleRect);

  painter.setPen(Qt::NoPen);
  QLinearGradient fade2(visibleRect.left(), visibleRect.top(), 
      visibleRect.left(), visibleRect.bottom());
  static const QColor fade2StartColor(0, 0, 0, 10);
  static const QColor fade2EndColor(0, 0, 0, 30);
  fade2.setColorAt(0, fade2StartColor);
  fade2.setColorAt(1, fade2EndColor);
  painter.setBrush(fade2);                
  painter.drawRect(visibleRect);

  painter.restore();
} // CommandButton::drawPressedBackground

void CommandButton::drawHotBackground(
    QStyleOption& buttonOption, QPainter& painter) const
{
  //todo
  //style()->drawPrimitive(
  //    QStyle::PE_PanelButtonTool, &buttonOption, &painter, this);
  painter.save();

  const int horMargin = 1 * logicalDpiX() / defaultDpiX;
  const int verMargin = 2 * logicalDpiY() / defaultDpiY;

  const int penWidth = 1 * logicalDpiX() / defaultDpiX;
  const QRect visibleRect = buttonOption.rect.adjusted(
      horMargin, verMargin, -horMargin, -verMargin);
  QPen pen;
  pen.setWidth(penWidth);
  static const QColor penColor(0, 0, 0, 128);
  pen.setColor(penColor);
  painter.setPen(pen);
  QLinearGradient fade1(visibleRect.left(), visibleRect.top(), 
      visibleRect.left(), visibleRect.bottom());
  static const QColor fade1StartColor(255, 255, 255, 60);
  static const QColor fade1EndColor(255, 255, 255, 5);
  fade1.setColorAt(0, fade1StartColor);
  fade1.setColorAt(1, fade1EndColor);
  painter.setBrush(fade1);                
  painter.drawRect(visibleRect);
  
  painter.setPen(Qt::NoPen);
  QLinearGradient fade2(visibleRect.left(), visibleRect.top(), 
      visibleRect.left(), visibleRect.bottom());
  static const QColor fade2StartColor(255, 255, 255, 80);
  static const QColor fade2EndColor(255, 255, 255, 0);
  fade2.setColorAt(0, fade2StartColor);
  fade2.setColorAt(1, fade2EndColor);
  painter.setBrush(fade2);                
  painter.drawRect(visibleRect);

  painter.restore();
} // CommandButton::drawHotBackground

void CommandButton::drawHotCheckedBackground(QStyleOption& buttonOption, 
    QPainter& painter) const
{
  drawHotBackground(buttonOption, painter);
} // CommandButton::drawHotCheckedBackground

void CommandButton::drawDisabledBackground(QStyleOption& /*buttonOption*/, 
    QPainter& /*painter*/) const
{
  //todo
  //style()->drawPrimitive(
  //    QStyle::PE_PanelButtonTool, &buttonOption, &painter, this);
} // CommandButton::drawDisabledBackground

void CommandButton::drawCheckedBackground(QStyleOption& buttonOption, 
    QPainter& painter) const
{
  //todo
  //style()->drawPrimitive(
  //    QStyle::PE_PanelButtonTool, &buttonOption, &painter, this);
  drawPressedBackground(buttonOption, painter);
} // CommandButton::drawCheckedBackground

} // namespace chrono
} // namespace ma

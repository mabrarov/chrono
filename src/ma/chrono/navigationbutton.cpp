/*
TRANSLATOR ma::chrono::NavigationButton
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <QStyle>
#include <QPainter>
#include <QStyleOptionToolButton>
#include <QLinearGradient>
#include <ma/chrono/constants.h>
#include <ma/chrono/navigationbutton.h>

namespace ma
{
  namespace chrono
  {
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
      QRect buttonRect = style->subControlRect(QStyle::CC_ToolButton, &buttonOption, QStyle::SC_ToolButton, this);      

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
        if (flags & (QStyle::State_Sunken | QStyle::State_On | QStyle::State_Raised)) 
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

    void NavigationButton::drawBackground(QStyleOption& option, QPainter& painter) const
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
      else if (option.state & QStyle::State_MouseOver || option.state & QStyle::State_HasFocus)
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

    void NavigationButton::drawNormalBackground(QStyleOption& /*option*/, QPainter& /*painter*/) const
    {
      // Do here nothing 
    } // NavigationButton::drawNormalBackground

    void NavigationButton::drawPressedBackground(QStyleOption& option, QPainter& painter) const
    {
      //todo
      //style()->drawPrimitive(QStyle::PE_PanelButtonTool, &option, &painter, this);
      painter.save();      
      qreal xRadius = static_cast<qreal>(3) * logicalDpiX() / defaultDpiX;        
      qreal yRadius = static_cast<qreal>(3) * logicalDpiY() / defaultDpiY;        
      int penWidth = 1 * logicalDpiX() / defaultDpiX;
      QRect outerRect = option.rect;
      QRect innerRect = outerRect.adjusted(xRadius - penWidth - 1, yRadius - penWidth - 1, -xRadius + penWidth + 1, -yRadius + penWidth + 1);
      QPen pen;
      pen.setWidth(penWidth);
      pen.setColor(QColor(0, 0, 0, 120));
      painter.setPen(pen);
      QLinearGradient fade1(outerRect.left(), outerRect.top(), outerRect.left(), outerRect.bottom());
      fade1.setColorAt(0, QColor(0, 0, 0, 100));
      fade1.setColorAt(1, QColor(0, 0, 0, 60));
      painter.setBrush(fade1);                
      painter.drawRoundedRect(innerRect, xRadius, yRadius);       

      xRadius = static_cast<qreal>(1) * logicalDpiX() / defaultDpiX;        
      yRadius = static_cast<qreal>(1) * logicalDpiY() / defaultDpiY;        
      qreal shiftX = static_cast<qreal>(1) * logicalDpiX() / defaultDpiX;        
      qreal shiftY = static_cast<qreal>(1) * logicalDpiY() / defaultDpiY;        
      outerRect = innerRect.adjusted(shiftX, shiftY, -shiftX, -shiftY);
      innerRect = outerRect.adjusted(xRadius - penWidth, yRadius - penWidth, -xRadius + penWidth, -yRadius + penWidth);
      painter.setPen(Qt::NoPen);
      QLinearGradient fade2(outerRect.left(), outerRect.top(), outerRect.left(), outerRect.bottom());
      fade2.setColorAt(0, QColor(255, 255, 255, 5));
      fade2.setColorAt(0.8, QColor(255, 255, 255, 100));
      fade2.setColorAt(1, QColor(255, 255, 255, 60));
      painter.setBrush(fade2);                
      painter.drawRoundedRect(innerRect, xRadius, yRadius);       
      painter.restore();
    } // NavigationButton::drawPressedBackground

    void NavigationButton::drawHotBackground(QStyleOption& option, QPainter& painter) const
    {
      //todo
      //style()->drawPrimitive(QStyle::PE_PanelButtonTool, &option, &painter, this);
      painter.save();      
      qreal xRadius = static_cast<qreal>(3) * logicalDpiX() / defaultDpiX;        
      qreal yRadius = static_cast<qreal>(3) * logicalDpiY() / defaultDpiY;        
      int penWidth = 1 * logicalDpiX() / defaultDpiX;
      QRect outerRect = option.rect;
      QRect innerRect = outerRect.adjusted(xRadius - penWidth - 1, yRadius - penWidth - 1, -xRadius + penWidth + 1, -yRadius + penWidth + 1);
      QPen pen;
      pen.setWidth(penWidth);
      pen.setColor(QColor(0, 0, 0, 80));
      painter.setPen(pen);
      QLinearGradient fade1(outerRect.left(), outerRect.top(), outerRect.left(), outerRect.bottom());
      fade1.setColorAt(0, QColor(0, 0, 0, 10));
      fade1.setColorAt(1, QColor(0, 0, 0, 50));
      painter.setBrush(fade1);                
      painter.drawRoundedRect(innerRect, xRadius, yRadius);       

      xRadius = static_cast<qreal>(1) * logicalDpiX() / defaultDpiX;        
      yRadius = static_cast<qreal>(1) * logicalDpiY() / defaultDpiY;        
      qreal shiftX = static_cast<qreal>(1) * logicalDpiX() / defaultDpiX;        
      qreal shiftY = static_cast<qreal>(1) * logicalDpiY() / defaultDpiY;        
      outerRect = innerRect.adjusted(shiftX, shiftY, -shiftX, -shiftY);
      innerRect = outerRect.adjusted(xRadius - penWidth, yRadius - penWidth, -xRadius + penWidth, -yRadius + penWidth);
      painter.setPen(Qt::NoPen);
      QLinearGradient fade2(outerRect.left(), outerRect.top(), outerRect.left(), outerRect.bottom());
      fade2.setColorAt(0, QColor(255, 255, 255, 90));
      fade2.setColorAt(0.5, QColor(255, 255, 255, 140));
      fade2.setColorAt(1, QColor(255, 255, 255, 0));
      painter.setBrush(fade2);                
      painter.drawRoundedRect(innerRect, xRadius, yRadius);       
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
} //namespace ma
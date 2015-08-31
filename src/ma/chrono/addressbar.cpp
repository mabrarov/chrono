/*
TRANSLATOR ma::chrono::AddressBar 
*/

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <utility>
#include <QSize>
#include <QPainter>
#include <QLinearGradient>
#include <QSizePolicy>
#include <QIcon>
#include <QStyleOption>
#include <QHBoxLayout>
#include <QToolButton>
#include <ma/chrono/constants.h>
#include <ma/chrono/navigationbutton.h>
#include <ma/chrono/breadcrumbbar.h>
#include <ma/chrono/addressbar.h>

namespace ma {
namespace chrono {    

AddressBar::AddressBar(QWidget* parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_NoSystemBackground);
  setAttribute(Qt::WA_OpaquePaintEvent);

  int horMargin = 6 * logicalDpiX() / defaultDpiX;      
  int verMargin = 4 * logicalDpiY() / defaultDpiY;            
  setContentsMargins(horMargin, verMargin, horMargin, verMargin);        
      
  const int horSpacing = 2 * logicalDpiX() / defaultDpiX;      
  const QSize iconSize(20 * logicalDpiX() / defaultDpiX, 
      20 * logicalDpiY() / defaultDpiY);

  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);        
  layout->setSpacing(horSpacing);

  backButton_ = new NavigationButton(this);
  backButton_->setIconSize(iconSize);
  backButton_->setToolButtonStyle(Qt::ToolButtonIconOnly);
  backButton_->setAutoRaise(true);      

  forwardButton_ = new NavigationButton(this);
  forwardButton_->setIconSize(iconSize);
  forwardButton_->setToolButtonStyle(Qt::ToolButtonIconOnly);
  forwardButton_->setAutoRaise(true);            

  refreshButton_ = new NavigationButton(this);
  refreshButton_->setIconSize(iconSize);
  refreshButton_->setToolButtonStyle(Qt::ToolButtonIconOnly);
  refreshButton_->setAutoRaise(true);            

  addressBreadCrumbBar_ = new BreadCrumbBar(this);  
  const int addressBreadCrumbBarRecommenedHeight = 
      24 * logicalDpiY() / defaultDpiY;
  addressBreadCrumbBar_->setMinimumHeight((std::max)(
      addressBreadCrumbBar_->minimumHeight(), 
      addressBreadCrumbBarRecommenedHeight));

  layout->addWidget(backButton_);
  layout->addWidget(forwardButton_);
  layout->addWidget(addressBreadCrumbBar_);      
  layout->addWidget(refreshButton_);

  setTabOrder(backButton_, forwardButton_);
  setTabOrder(forwardButton_, addressBreadCrumbBar_);      
  setTabOrder(addressBreadCrumbBar_, refreshButton_);      

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

AddressBar::~AddressBar()
{
}

NavigationButton* AddressBar::backButton() const
{
  return backButton_;
}

NavigationButton* AddressBar::forwardButton() const
{
  return forwardButton_;
}    

NavigationButton* AddressBar::refreshButton() const
{
  return refreshButton_;
}    

BreadCrumbBar* AddressBar::addressBreadCrumbBar() const
{
  return addressBreadCrumbBar_;
}

void AddressBar::paintEvent(QPaintEvent* /*event*/)
{
  QPainter painter(this);
  QLinearGradient fade(0, 0, 0, height());
  fade.setColorAt(0, QColor(237, 237, 237, 255));
  fade.setColorAt(1, QColor(210, 210, 210, 255));
  painter.fillRect(rect(), fade);      
}

} // namespace chrono
} // namespace ma

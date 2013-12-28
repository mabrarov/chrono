//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_ADDRESSBAR_H
#define MA_CHRONO_ADDRESSBAR_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <QWidget>
#include <ma/chrono/navigationbutton_fwd.h>
#include <ma/chrono/breadcrumbbar_fwd.h>
#include <ma/chrono/addressbar_fwd.h>

namespace ma {
namespace chrono {    

class AddressBar : public QWidget
{
  Q_OBJECT

public:
  explicit AddressBar(QWidget* parent);
  ~AddressBar(); 
  NavigationButton* backButton() const;
  NavigationButton* forwardButton() const;      
  NavigationButton* refreshButton() const;      
  BreadCrumbBar*    addressBreadCrumbBar() const;

protected:
  void paintEvent(QPaintEvent* event);

private:
  Q_DISABLE_COPY(AddressBar)        

  NavigationButton* backButton_;
  NavigationButton* forwardButton_;      
  NavigationButton* refreshButton_;      
  BreadCrumbBar*    addressBreadCrumbBar_;
}; // class AddressBar

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_ADDRESSBAR_H

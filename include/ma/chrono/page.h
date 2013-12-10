//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_PAGE_H
#define MA_CHRONO_PAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <QWidget>
#include <ma/chrono/types.h>
#include <ma/chrono/resourcemanager_fwd.h>
#include <ma/chrono/taskwindowmanager_fwd.h>
#include <ma/chrono/page_fwd.h>

namespace ma {
namespace chrono {

class Page : public QWidget
{
  Q_OBJECT

public:
  Page(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const QActionSharedPtr& helpAction,
      QWidget* parent = 0);
  ~Page();
  ResourceManagerPtr resourceManager() const;      
  TaskWindowManagerWeakPtr taskWindowManager() const;
  QActionSharedPtr helpAction() const;
  virtual QActionList primaryActions() const = 0;
  virtual QAction*    customizeAction() const = 0;
  virtual QActionList contextActions() const = 0;
      
private:
  Q_DISABLE_COPY(Page)
            
  ResourceManagerPtr resourceManager_;
  TaskWindowManagerWeakPtr taskWindowManager_;
  QActionSharedPtr helpAction_;
}; // class Page

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_PAGE_H

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_TASKWINDOWMANAGER_H
#define MA_CHRONO_TASKWINDOWMANAGER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QMap>
#include <ma/chrono/dbconstants.h>
#include <ma/chrono/listtaskwindow_fwd.h>
#include <ma/chrono/cardtaskwindow_fwd.h>
#include <ma/chrono/taskwindowmanager_fwd.h>

namespace ma {
namespace chrono {    

class TaskWindowManager : public QObject
{     
  Q_OBJECT

public:
  TaskWindowManager();
  ~TaskWindowManager();      
  void add(const boost::shared_ptr<ListTaskWindow>& listTaskWindow);
  void add(const boost::shared_ptr<CardTaskWindow>& cardTaskWindow);
  boost::shared_ptr<CardTaskWindow> findCardTaskWindow(Entity entity, 
      qint64 entityId);

private:
  Q_DISABLE_COPY(TaskWindowManager)
      
  QMap<ListTaskWindow*, boost::shared_ptr<ListTaskWindow> > listTaskWindows_;
  QMap<CardTaskWindow*, boost::shared_ptr<CardTaskWindow> > cardTaskWindows_;      

private slots:
  void removeListTaskWindow(int);
  void removeCardTaskWindow(int);
}; // class TaskWindowManager

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_TASKWINDOWMANAGER_H

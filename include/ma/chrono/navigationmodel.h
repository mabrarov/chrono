//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_NAVIGATIONMODEL_H
#define MA_CHRONO_NAVIGATIONMODEL_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/config.hpp>
#include <qglobal.h>
#include <QStandardItemModel>
#include <ma/chrono/types.h>
#include <ma/chrono/navigationmodel_fwd.h>
#include <ma/chrono/resourcemanager_fwd.h> 
#include <ma/chrono/taskwindowmanager_fwd.h> 
#include <ma/chrono/databasemodel_fwd.h>

QT_BEGIN_NAMESPACE
class QWidget;  
class QAction;
QT_END_NAMESPACE

namespace ma {
namespace chrono {

class NavigationModel : public QStandardItemModel
{
  Q_OBJECT

public:
  BOOST_STATIC_CONSTANT(Qt::ItemDataRole, pageItemDataRole = Qt::UserRole); 

  NavigationModel(QObject* parent, QWidget* pageParent,
      const QActionSharedPtr& helpAction, 
      const ResourceManagerPtr& resourceManager, 
      const TaskWindowManagerPtr& taskWindowManager, 
      const DatabaseModelPtr& databaseModel,
      const QSize& itemSizeHint);
  ~NavigationModel();

private:
  Q_DISABLE_COPY(NavigationModel)

  static void populate(QWidget* pageParent,
      const QActionSharedPtr& helpAction, 
      QStandardItemModel& navigationModel,
      const ResourceManagerPtr& resourceManager, 
      const TaskWindowManagerPtr& taskWindowManager, 
      const DatabaseModelPtr& databaseModel,
      const QSize& itemSizeHint);
  static void populateMain(QWidget* pageParent, 
      const QActionSharedPtr& helpAction,
      QStandardItem& parentItem, 
      const ResourceManagerPtr& resourceManager, 
      const TaskWindowManagerPtr& taskWindowManager, 
      const DatabaseModelPtr& databaseModel,
      const QSize& itemSizeHint);
  static void populateReferences(QWidget* pageParent, 
      const QActionSharedPtr& helpAction,
      QStandardItem& parentItem, 
      const ResourceManagerPtr& resourceManager, 
      const TaskWindowManagerPtr& taskWindowManager, 
      const DatabaseModelPtr& databaseModel,
      const QSize& itemSizeHint);
  static void populateService(QWidget* pageParent,
      const QActionSharedPtr& helpAction,
      QStandardItem& parentItem, 
      const ResourceManagerPtr& resourceManager, 
      const TaskWindowManagerPtr& taskWindowManager, 
      const DatabaseModelPtr& databaseModel,
      const QSize& itemSizeHint);      
}; // class QStandardItemModel

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_NAVIGATIONMODEL_H

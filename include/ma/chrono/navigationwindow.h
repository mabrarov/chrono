//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_NAVIGATIONWINDOW_H
#define MA_CHRONO_NAVIGATIONWINDOW_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <memory>
#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>
#include <Qt>
#include <QVector>
#include <QModelIndex>
#include <QMainWindow>
#include <ma/chrono/addressbar_fwd.h>
#include <ma/chrono/commandbar_fwd.h>
#include <ma/chrono/resourcemanager_fwd.h>
#include <ma/chrono/taskwindowmanager_fwd.h>
#include <ma/chrono/databasemodel_fwd.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/navigationmodel_fwd.h>
#include <ma/chrono/page_fwd.h>
#include <ma/chrono/historylist.h>
#include <ma/chrono/databaseconnectdialog_fwd.h>

QT_BEGIN_NAMESPACE
class QPoint;
class QStandardItemModel;
class QBoxLayout;
class QToolBox;
class QLabel;
class QStatusBar;
class QTreeView;  
class QMenu;
QT_END_NAMESPACE

namespace ma {
namespace chrono {

class NavigationWindow : public QMainWindow
{
  Q_OBJECT

public:
  NavigationWindow();
  ~NavigationWindow();      

protected:
  void closeEvent(QCloseEvent* event); 

private:            
  Q_DISABLE_COPY(NavigationWindow)

  void connectListPages(QStandardItemModel& navigationModel);

  static boost::tuple<QVector<QTreeView*>, QVector<QModelIndex> > 
  populateNavigationPane(QWidget* parent, QToolBox& navigationPane, 
      QStandardItemModel& navigationModel, const QSize& iconSize);

  void setActivePage(const QModelIndex& index);
  void setActivePage(Page* page);

  boost::optional<QModelIndex> findNavigationTreeRoot(
      const QModelIndex& index) const;
  void updateUserName();
  void updateDatabaseConnectionName();

  ResourceManagerPtr resourceManager_;
  TaskWindowManagerPtr taskWindowManager_;
  DatabaseModelPtr databaseModel_;
  NavigationModel* navigationModel_;
  HistoryList<QModelIndex> navigationHistory_;
  QStatusBar* statusBar_;
  AddressBar* addressBar_; 
  CommandBar* commandBar_;
  QToolBox* navigationPane_;    
  QLabel* databaseConnectionStateLabel_;
  QLabel* userNameLabel_;
  QVector<QTreeView*> navigationTrees_;
  QVector<QModelIndex> navigationTreeRoots_;      
  QBoxLayout* pageLayout_;
  QAction* goBackAction_;
  QAction* goForwardAction_;
  QAction* refreshAction_;
  QAction* connectDatabaseAction_;
  QAction* disconnectDatabaseAction_;      
  QModelIndex activeNavigationIndex_;      
  Page* activePage_; 
  DatabaseConnectDialog* databaseConnectDialog_;
  std::auto_ptr<QMenu> helpMenu_;

private slots:
  void on_navigationPane_currentChanged(int index);
  void on_navigationTree_contextMenuRequested(const QPoint& pos);
  void on_navigationTree_currentChanged(const QModelIndex& current, 
      const QModelIndex& previous);      
  void on_goBackAction_triggered();
  void on_goForwardAction_triggered();      
  void on_refreshAction_triggered();      
  void on_addressBreadCrumbBar_selected(const QModelIndex& index);
  void on_connectDatabaseAction_triggered();
  void on_disconnectDatabaseAction_triggered();      
  void on_databaseModel_connectionStateChanged();      
  void on_aboutChronoAction_triggered(); 
  void on_listPage_doubleClicked(const QModelIndex& index);
  void on_userUpdated(const ma::chrono::model::User& user);
}; // class NavigationWindow

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_NAVIGATIONWINDOW_H

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_LISTPAGE_H
#define MA_CHRONO_LISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <memory>
#include <boost/optional.hpp>
#include <ma/chrono/types.h>
#include <ma/chrono/dbconstants.h>
#include <ma/chrono/datapage.h>
#include <ma/chrono/cardtaskwindow_fwd.h>
#include <ma/chrono/cardpage.h>
#include <ma/chrono/userdefinedfilter.h>
#include <ma/chrono/userdefinedorder.h>
#include <ma/chrono/tabledescription.h>
#include <ma/chrono/filterdialog_fwd.h>
#include <ma/chrono/sortdialog_fwd.h>

QT_BEGIN_NAMESPACE
class QMenu;
class QFrame;
class QTableView;
class QModelIndex;
class QAbstractItemModel;
class QAbstractButton;
QT_END_NAMESPACE

namespace ma {
namespace chrono {

class ListPage : public DataPage
{
  Q_OBJECT

public:
  ListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Entity entity, 
      const TableDescription& tableDescription,
      const UserDefinedFilter& userDefinedFilter,  
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent);
  ~ListPage();    

  QTableView* tableView() const;
  Entity entity() const;      
  QAbstractItemModel* model() const;
  void setModel(QAbstractItemModel* model);      

  QActionList primaryActions() const;
  QAction*    customizeAction() const;
  QActionList contextActions() const;
      
  QAction* viewAction() const;
  QAction* editAction() const;
  QAction* createAction() const;
  QAction* deleteAction() const;
  QAction* openInNewWindowAction() const;
  QAction* refreshAction() const;
  //QAction* clearFilterAction() const;
  QAction* sendToExcelAction() const;
  QAction* printPageAction() const;      

  void addPrimaryActions(const QActionList& actions);
  void addSpecialActions(const QActionList& actions);

  virtual void viewSelected();
  virtual void editSelected();      
  virtual void createNew();
  virtual void removeSelected();
  OptionalQInt64   selectedId() const;
  OptionalQString  selectedRelationalText() const;
  TableDescription tableDescription() const;

  virtual std::auto_ptr<ListPage> clone(QWidget* parent = 0) const = 0;
  virtual std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;
  virtual OptionalQInt64  entityIdFromRow(int row) const;
  virtual OptionalQString relationalTextFromRow(int row) const;

  void setUserDefinedFilter(const UserDefinedFilter& userDefinedFilter);
  UserDefinedFilter userDefinedFilter() const;
  void setUserDefinedOrder(const UserDefinedOrder& userDefinedOrder);
  UserDefinedOrder userDefinedOrder() const;

signals:
  void currentChanged(const QModelIndex& current, const QModelIndex& previous);
  void doubleClicked(const QModelIndex& index);

protected:
  virtual void applyUserDefinedFilter(
      const UserDefinedFilter& userDefinedFilter) = 0;
  virtual void applyUserDefinedOrder(
      const UserDefinedOrder& userDefinedOrder) = 0;
            
private:
  Q_DISABLE_COPY(ListPage)

  void updateState();
  void updateActions();
  boost::shared_ptr<CardTaskWindow> showSelectedCardTaskWindow(
      CardPage::Mode mode) const;      
                
  Entity      entity_;
  QMenu*      actionsMenu_;
  QAction*    viewAction_;
  QAction*    editAction_;
  QAction*    createAction_;
  QAction*    deleteAction_;
  QAction*    processActionsSeparator_;      
  QAction*    specialActionsSeparator_;
  QAction*    openInNewWindowAction_;
  QAction*    refreshAction_;
  //QAction* clearFilterAction_;
  QAction*    sendToExcelAction_;      
  QAction*    commonActionsSeparator_;
  QAction*    printPageAction_;
  QActionList primaryActions_;
  QAction*    customizeAction_;
  QActionList contextActions_;
  QMenu*      rowContextMenu_;
  QActionList dataAwareActions_;
  QActionList itemAwareActions_;
  QFrame*     filterPane_;
      
  QAbstractButton* enableFilterBtn_;
  QAbstractButton* customizeFilterBtn_;
  QAbstractButton* enableSortBtn_;
  QAbstractButton* customizeSortBtn_;
      
  QTableView*       tableView_;
  FilterDialog*     filterDialog_; 
  SortDialog*       sortDialog_;
  TableDescription  tableDescription_;
  UserDefinedFilter userDefinedFilter_;
  UserDefinedOrder  userDefinedOrder_;

private slots:
  void on_viewAction_triggered();
  void on_editAction_triggered();
  void on_createAction_triggered();
  void on_deleteAction_triggered();
  void on_openInNewWindowAction_triggered();      
  void on_refreshAction_triggered();      
  //void on_clearFilterAction_triggered();
  void on_sendToExcelAction_triggered();
  void on_printPageAction_triggered();
  void on_headerContextMenu_requested(const QPoint& pos);
  void on_tableViewContextMenu_requested(const QPoint& pos);
  void on_databaseModel_connectionStateChanged();
  void on_currentChanged(const QModelIndex& current, 
      const QModelIndex& previous);
  void on_enableFilterBtn_clicked(bool checked = false);
  void on_customizeFilterBtn_clicked(bool checked = false);
  void on_enableSortBtn_clicked(bool checked = false);
  void on_customizeSortBtn_clicked(bool checked = false);
}; // class ListPage    

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_LISTPAGE_H

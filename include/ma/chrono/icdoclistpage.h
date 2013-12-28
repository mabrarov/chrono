//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_ICDOCLISTPAGE_H
#define MA_CHRONO_ICDOCLISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/icdoctablemodel_fwd.h>
#include <ma/chrono/model/icdoc_fwd.h>
//#include <ma/chrono/model/contract_fwd.h>
//#include <ma/chrono/model/counterpart_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/listpage.h>

namespace ma {
namespace chrono {

class IcdocListPage : public ListPage
{
  Q_OBJECT

public:
  IcdocListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalInt& filterId,
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent);
  ~IcdocListPage();

  void refresh();
  std::auto_ptr<ListPage> clone(QWidget* parent = 0) const;      
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;      
  OptionalQString relationalTextFromRow(int row) const;

protected:
  void applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter);
  void applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder);

private:
  Q_DISABLE_COPY(IcdocListPage)                      

  IcdocTableModel* model_;
  OptionalInt filterId_;

private slots:
  void on_database_connectionStateChanged();
  void on_database_aboutToClose();      
  void on_database_icdocInserted(const ma::chrono::model::Icdoc& icdoc);
  void on_database_icdocUpdated(const ma::chrono::model::Icdoc& icdoc);
  void on_database_icdocRemoved(const ma::chrono::model::Icdoc& icdoc);
  //void on_database_contractUpdated(
  //    const ma::chrono::model::Contract& contract);
  //void on_database_counterpartUpdated(
  //    const ma::chrono::model::Counterpart& counterpart);
  void on_database_userUpdated(const ma::chrono::model::User& user);
}; // class IcdocListPage

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_ICDOCLISTPAGE_H

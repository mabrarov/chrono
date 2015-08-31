//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_ICTLISTPAGE_H
#define MA_CHRONO_ICTLISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/icttablemodel_fwd.h>
#include <ma/chrono/model/ict_fwd.h>
//#include <ma/chrono/model/contract_fwd.h>
//#include <ma/chrono/model/counterpart_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/listpage.h>

namespace ma {
namespace chrono {

class IctListPage : public ListPage
{
  Q_OBJECT

public:
  IctListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalInt& filterId,
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent);
  ~IctListPage();

  void refresh();
  std::auto_ptr<ListPage> clone(QWidget* parent = 0) const;      
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;      
  OptionalQString relationalTextFromRow(int row) const;

protected:
  void applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter);
  void applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder);

private:
  Q_DISABLE_COPY(IctListPage)                      

  IctTableModel* model_;
  OptionalInt filterId_;

private slots:
  void on_database_connectionStateChanged();
  void on_database_aboutToClose();      
  void on_database_ictInserted(const ma::chrono::model::Ict& ict);
  void on_database_ictUpdated(const ma::chrono::model::Ict& ict);
  void on_database_ictRemoved(const ma::chrono::model::Ict& ict);
  //void on_database_contractUpdated(
  //    const ma::chrono::model::Contract& contract);
  //void on_database_counterpartUpdated(
  //    const ma::chrono::model::Counterpart& counterpart);
  void on_database_userUpdated(const ma::chrono::model::User& user);            
}; // class IctListPage

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_ICTLISTPAGE_H

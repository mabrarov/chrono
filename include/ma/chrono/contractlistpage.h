//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_CONTRACTLISTPAGE_H
#define MA_CHRONO_CONTRACTLISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/contracttablemodel_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/bank_fwd.h>
#include <ma/chrono/model/resident_fwd.h>
#include <ma/chrono/model/counterpart_fwd.h>
#include <ma/chrono/model/currency_fwd.h>
#include <ma/chrono/model/contract_fwd.h>
#include <ma/chrono/listpage.h>

namespace ma {
namespace chrono {

class ContractListPage : public ListPage
{
  Q_OBJECT

public:
  ContractListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalInt& filterId,
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent);
  ~ContractListPage();

  void refresh();
  std::auto_ptr<ListPage> clone(QWidget* parent = 0) const;      
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;            

protected:
  void applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter);
  void applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder);

private:
  Q_DISABLE_COPY(ContractListPage)

  ContractTableModel* model_;
  OptionalInt filterId_;

private slots:
  void on_database_connectionStateChanged();
  void on_database_aboutToClose();
  void on_database_userUpdated(const ma::chrono::model::User& user);
  void on_database_bankUpdated(const ma::chrono::model::Bank& bank);
  void on_database_residentUpdated(
      const ma::chrono::model::Resident& resident);
  void on_database_counterpartUpdated(
      const ma::chrono::model::Counterpart& counterpart);
  void on_database_currencyUpdated(
      const ma::chrono::model::Currency& currency);
  void on_database_contractInserted(
      const ma::chrono::model::Contract& contract);
  void on_database_contractUpdated(
      const ma::chrono::model::Contract& contract);
  void on_database_contractRemoved(
      const ma::chrono::model::Contract& contract);
}; // class ContractListPage

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_CONTRACTLISTPAGE_H

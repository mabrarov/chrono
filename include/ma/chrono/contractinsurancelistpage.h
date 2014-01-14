//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_CONTRACTINSURANCELISTPAGE_H
#define MA_CHRONO_CONTRACTINSURANCELISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/optional.hpp>
#include <ma/chrono/contractinsurancetablemodel_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/insurance_fwd.h>
#include <ma/chrono/model/contract_fwd.h>
#include <ma/chrono/model/currency_fwd.h>
#include <ma/chrono/listpage.h>

namespace ma {
namespace chrono {

class ContractInsuranceListPage : public ListPage
{
  Q_OBJECT

public:
  ContractInsuranceListPage(
      const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalQInt64& contractId, 
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,        
      QWidget* parent);
  ~ContractInsuranceListPage();

  void refresh();
  std::auto_ptr<ListPage> clone(QWidget* parent = 0) const;      
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;      
  void setContractId(qint64 contractId);
  QActionList parentActions() const;
  QActionList selectedParentActions() const;

protected:
  void applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter);
  void applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder);

private:
  Q_DISABLE_COPY(ContractInsuranceListPage)                      
      
  OptionalQInt64 contractId_;      
  ContractInsuranceTableModel* model_;
  QActionList actions_;
  QActionList selectedRowActions_;
  void updateContractRelatedData();

private slots:
  void on_database_connectionStateChanged();
  void on_database_aboutToClose();      
  void on_database_userUpdated(const ma::chrono::model::User& user);            
  void on_database_contractUpdated(
      const ma::chrono::model::Contract& contract);
  void on_database_contractRemoved(
      const ma::chrono::model::Contract& contract);
  void on_database_currencyUpdated(
      const ma::chrono::model::Currency& currency);
  void on_database_insuranceInserted(
      const ma::chrono::model::Insurance& insurance);
  void on_database_insuranceUpdated(
      const ma::chrono::model::Insurance& insurance);
  void on_database_insuranceRemoved(
      const ma::chrono::model::Insurance& insurance); 
      
}; // class ContractInsuranceListPage

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_CONTRACTINSURANCELISTPAGE_H

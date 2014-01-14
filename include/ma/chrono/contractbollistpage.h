//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_CONTRACTBOLLISTPAGE_H
#define MA_CHRONO_CONTRACTBOLLISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/optional.hpp>
#include <ma/chrono/contractboltablemodel_fwd.h>
#include <ma/chrono/model/bol_fwd.h>
#include <ma/chrono/model/ccd_fwd.h>
#include <ma/chrono/model/contract_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/autoenterprise_fwd.h>
#include <ma/chrono/listpage.h>
#include <ma/chrono/contractbollistpage_fwd.h>

namespace ma {
namespace chrono {

class ContractBolListPage : public ListPage
{
  Q_OBJECT

public:
  ContractBolListPage(
      const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalQInt64& contractId, 
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,        
      QWidget* parent);
  ~ContractBolListPage();

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
  Q_DISABLE_COPY(ContractBolListPage)                      
      
  OptionalQInt64 contractId_;
  ContractBolTableModel* model_;
  QActionList actions_;
  QActionList selectedRowActions_;
  void updateContractRelatedData();

private slots:
  void on_database_connectionStateChanged();
  void on_database_aboutToClose();            
  void on_database_contractUpdated(
      const ma::chrono::model::Contract& contract);
  void on_database_contractRemoved(
      const ma::chrono::model::Contract& contract);
  void on_database_bolInserted(const ma::chrono::model::Bol& bol);
  void on_database_bolUpdated(const ma::chrono::model::Bol& bol);
  void on_database_bolRemoved(const ma::chrono::model::Bol& bol);
  void on_database_ccdUpdated(const ma::chrono::model::Ccd& ccd);            
  void on_database_userUpdated(const ma::chrono::model::User& user);            
  void on_database_autoenterpriseUpdated(
      const ma::chrono::model::Autoenterprise& autoenterprise);
      
}; // class ContractBolListPage

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_CONTRACTBOLLISTPAGE_H

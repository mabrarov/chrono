//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_CONTRACTSBOLLISTPAGE_H
#define MA_CHRONO_CONTRACTSBOLLISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/optional.hpp>
#include <ma/chrono/contractsboltablemodel_fwd.h>
#include <ma/chrono/model/sbol_fwd.h>
#include <ma/chrono/model/ccd_fwd.h>
#include <ma/chrono/model/contract_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/lcredit_fwd.h>
#include <ma/chrono/listpage.h>
#include <ma/chrono/contractsbollistpage_fwd.h>

namespace ma {
namespace chrono {

class ContractSbolListPage : public ListPage
{
  Q_OBJECT

public:
  ContractSbolListPage(
      const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalQInt64& contractId, 
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,        
      QWidget* parent);
  ~ContractSbolListPage();

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
  Q_DISABLE_COPY(ContractSbolListPage)                      
      
  OptionalQInt64 contractId_;
  ContractSbolTableModel* model_;
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
  void on_database_sbolInserted(const ma::chrono::model::Sbol& sbol);
  void on_database_sbolUpdated(const ma::chrono::model::Sbol& sbol);
  void on_database_sbolRemoved(const ma::chrono::model::Sbol& sbol);
  void on_database_ccdUpdated(const ma::chrono::model::Ccd& ccd);
  void on_database_userUpdated(const ma::chrono::model::User& user);
  void on_database_lcreditUpdated(const ma::chrono::model::Lcredit& lcredit);
      
}; // class ContractSbolListPage

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_CONTRACTSBOLLISTPAGE_H

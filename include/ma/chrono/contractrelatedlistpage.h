//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_CONTRACTRELATEDLISTPAGE_H
#define MA_CHRONO_CONTRACTRELATEDLISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/optional.hpp>
#include <ma/chrono/model/contract_fwd.h>
#include <ma/chrono/listpage.h>

namespace ma {
namespace chrono {

class ContractRelatedListPage : public ListPage
{
  Q_OBJECT

public:
  ContractRelatedListPage(
      const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction,
      Entity entity,
      const TableDescription& tableDescription,
      const OptionalQInt64& contractId, 
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,        
      QWidget* parent);
  ~ContractRelatedListPage();      

  void setContractId(qint64 contractId);
  QActionList parentActions() const;
  QActionList selectedParentActions() const;

private:
  Q_DISABLE_COPY(ContractRelatedListPage)                      
      
  OptionalQInt64 contractId_;      
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
}; // class ContractRelatedListPage

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_CONTRACTRELATEDLISTPAGE_H

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_SBOLLISTPAGE_H
#define MA_CHRONO_SBOLLISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/sboltablemodel_fwd.h>
#include <ma/chrono/model/sbol_fwd.h>
#include <ma/chrono/model/ccd_fwd.h>
#include <ma/chrono/model/contract_fwd.h>
#include <ma/chrono/model/counterpart_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/resident_fwd.h>
#include <ma/chrono/model/lcredit_fwd.h>
#include <ma/chrono/listpage.h>

namespace ma {
namespace chrono {

class SbolListPage : public ListPage
{
  Q_OBJECT

public:
  SbolListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalInt& filterId,
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent);
  ~SbolListPage();

  void refresh();
  std::auto_ptr<ListPage> clone(QWidget* parent = 0) const;      
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;      

protected:
  void applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter);    
  void applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder);

private:
  Q_DISABLE_COPY(SbolListPage)                      

  SbolTableModel* model_;
  OptionalInt filterId_;

private slots:
  void on_database_connectionStateChanged();
  void on_database_aboutToClose();      
  void on_database_sbolInserted(const ma::chrono::model::Sbol& sbol);
  void on_database_sbolUpdated(const ma::chrono::model::Sbol& sbol);
  void on_database_sbolRemoved(const ma::chrono::model::Sbol& sbol);
  void on_database_ccdUpdated(const ma::chrono::model::Ccd& ccd);
  void on_database_contractUpdated(
      const ma::chrono::model::Contract& contract);
  void on_database_counterpartUpdated(
      const ma::chrono::model::Counterpart& counterpart);
  void on_database_userUpdated(const ma::chrono::model::User& user);
  void on_database_residentUpdated(
      const ma::chrono::model::Resident& resident);
  void on_database_lcreditUpdated(const ma::chrono::model::Lcredit& lcredit);
}; // class SbolListPage

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_SBOLLISTPAGE_H

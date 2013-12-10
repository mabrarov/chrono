//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_BOLLISTPAGE_H
#define MA_CHRONO_BOLLISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/types.h>
#include <ma/chrono/boltablemodel_fwd.h>
#include <ma/chrono/model/bol_fwd.h>
#include <ma/chrono/model/ccd_fwd.h>
#include <ma/chrono/model/contract_fwd.h>
#include <ma/chrono/model/counterpart_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/autoenterprise_fwd.h>
#include <ma/chrono/listpage.h>

namespace ma {
namespace chrono {

class BolListPage : public ListPage
{
  Q_OBJECT

public:
  BolListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalInt& filterId,
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent);
  ~BolListPage();

  void refresh();
  std::auto_ptr<ListPage> clone(QWidget* parent = 0) const;      
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;            

protected:
  void applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter);
  void applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder);

private:
  Q_DISABLE_COPY(BolListPage)                      

  BolTableModel* model_;
  OptionalInt    filterId_;

private slots:
  void on_database_connectionStateChanged();
  void on_database_aboutToClose();      
  void on_database_bolInserted(const ma::chrono::model::Bol& bol);
  void on_database_bolUpdated(const ma::chrono::model::Bol& bol);
  void on_database_bolRemoved(const ma::chrono::model::Bol& bol);
  void on_database_ccdUpdated(const ma::chrono::model::Ccd& ccd);
  void on_database_contractUpdated(
      const ma::chrono::model::Contract& contract);
  void on_database_counterpartUpdated(
      const ma::chrono::model::Counterpart& counterpart);
  void on_database_userUpdated(const ma::chrono::model::User& user);
  void on_database_autoenterpriseUpdated(
      const ma::chrono::model::Autoenterprise& autoenterprise);            
}; // class BolListPage

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_BOLLISTPAGE_H

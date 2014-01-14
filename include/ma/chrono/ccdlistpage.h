//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_CCDLISTPAGE_H
#define MA_CHRONO_CCDLISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/ccdtablemodel_fwd.h>
#include <ma/chrono/model/ccd_fwd.h>
#include <ma/chrono/model/contract_fwd.h>
#include <ma/chrono/model/counterpart_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/deppoint_fwd.h>
#include <ma/chrono/model/bank_fwd.h>
#include <ma/chrono/model/tterm_fwd.h>
#include <ma/chrono/model/currency_fwd.h>
#include <ma/chrono/listpage.h>

namespace ma {
namespace chrono {

class CcdListPage : public ListPage
{
  Q_OBJECT

public:
  CcdListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalInt& filterId,
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent);
  ~CcdListPage();

  void refresh();
  std::auto_ptr<ListPage> clone(QWidget* parent = 0) const;      
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;

protected:
  void applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter);
  void applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder);

private:
  Q_DISABLE_COPY(CcdListPage)                      

  CcdTableModel* model_;
  OptionalInt filterId_;

private slots:
  void on_database_connectionStateChanged();
  void on_database_aboutToClose();      
  void on_database_ccdInserted(const ma::chrono::model::Ccd& ccd);
  void on_database_ccdUpdated(const ma::chrono::model::Ccd& ccd);
  void on_database_ccdRemoved(const ma::chrono::model::Ccd& ccd);
  void on_database_contractUpdated(
      const ma::chrono::model::Contract& contract);
  void on_database_counterpartUpdated(
      const ma::chrono::model::Counterpart& counterpart);
  void on_database_userUpdated(const ma::chrono::model::User& user);
  void on_database_deppointUpdated(
      const ma::chrono::model::Deppoint& deppoint);
  void on_database_bankUpdated(const ma::chrono::model::Bank& bank);
  void on_database_ttermUpdated(const ma::chrono::model::Tterm& tterm);
  void on_database_currencyUpdated(
      const ma::chrono::model::Currency& currency);
      
}; // class CcdListPage

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_CCDLISTPAGE_H

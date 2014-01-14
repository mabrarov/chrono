//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_COUNTERPARTLISTPAGE_H
#define MA_CHRONO_COUNTERPARTLISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/counterparttablemodel_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/country_fwd.h>
#include <ma/chrono/model/counterpart_fwd.h>
#include <ma/chrono/listpage.h>

namespace ma {
namespace chrono {

class CounterpartListPage : public ListPage
{
  Q_OBJECT

public:
  CounterpartListPage(
      const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent);
  ~CounterpartListPage();

  void refresh();
  std::auto_ptr<ListPage> clone(QWidget* parent = 0) const;
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;     
  OptionalQString relationalTextFromRow(int row) const;

protected:
  void applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter);
  void applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder);

private:
  Q_DISABLE_COPY(CounterpartListPage)                      

  CounterpartTableModel* model_;

private slots:
  void on_database_connectionStateChanged();
  void on_database_aboutToClose();
  void on_database_userUpdated(const ma::chrono::model::User& user);
  void on_database_countryUpdated(const ma::chrono::model::Country& country);
  void on_database_counterpartInserted(
      const ma::chrono::model::Counterpart& counterpart);
  void on_database_counterpartUpdated(
      const ma::chrono::model::Counterpart& counterpart);
  void on_database_counterpartRemoved(
      const ma::chrono::model::Counterpart& counterpart);
}; // class CounterpartListPage

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_COUNTERPARTLISTPAGE_H

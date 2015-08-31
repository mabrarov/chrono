//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_RESIDENTLISTPAGE_H
#define MA_CHRONO_RESIDENTLISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/residenttablemodel_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/country_fwd.h>
#include <ma/chrono/model/resident_fwd.h>
#include <ma/chrono/listpage.h>

namespace ma {
namespace chrono {

class ResidentListPage : public ListPage
{
  Q_OBJECT

public:
  ResidentListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction,         
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent);
  ~ResidentListPage();

  void refresh();
  std::auto_ptr<ListPage> clone(QWidget* parent = 0) const;
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;      
  OptionalQString relationalTextFromRow(int row) const;

protected:
  void applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter);
  void applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder);

private:
  Q_DISABLE_COPY(ResidentListPage)                      

  ResidentTableModel* model_;

private slots:
  void on_database_connectionStateChanged();
  void on_database_aboutToClose();      
  void on_database_userUpdated(const ma::chrono::model::User& user);            
  void on_database_countryUpdated(const ma::chrono::model::Country& country);      
  void on_database_residentInserted(
      const ma::chrono::model::Resident& resident);
  void on_database_residentUpdated(
      const ma::chrono::model::Resident& resident);
  void on_database_residentRemoved(
      const ma::chrono::model::Resident& resident);
}; // class ResidentListPage

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_RESIDENTLISTPAGE_H

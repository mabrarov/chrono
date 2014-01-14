//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_DEPPOINTLISTPAGE_H
#define MA_CHRONO_DEPPOINTLISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/deppointtablemodel_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/deppoint_fwd.h>
#include <ma/chrono/listpage.h>

namespace ma {
namespace chrono {

class DeppointListPage : public ListPage
{
  Q_OBJECT

public:
  DeppointListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent);
  ~DeppointListPage();

  void refresh();
  std::auto_ptr<ListPage> clone(QWidget* parent = 0) const;
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;
  OptionalQString relationalTextFromRow(int row) const;

protected:
  void applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter);
  void applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder);

private:
  Q_DISABLE_COPY(DeppointListPage)                      

  DeppointTableModel* model_;

private slots:
  void on_database_connectionStateChanged();
  void on_database_aboutToClose();      
  void on_database_userUpdated(const ma::chrono::model::User& user);      
  void on_database_deppointInserted(
      const ma::chrono::model::Deppoint& deppoint);
  void on_database_deppointUpdated(
      const ma::chrono::model::Deppoint& deppoint);
  void on_database_deppointRemoved(
      const ma::chrono::model::Deppoint& deppoint);
}; // class DeppointListPage

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_DEPPOINTLISTPAGE_H

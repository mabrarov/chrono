//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_BANKLISTPAGE_H
#define MA_CHRONO_BANKLISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/banktablemodel_fwd.h>
#include <ma/chrono/model/bank_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/listpage.h>

namespace ma {
namespace chrono {

class BankListPage : public ListPage
{
  Q_OBJECT

public:
  BankListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent);      
  ~BankListPage();

  void refresh();
  std::auto_ptr<ListPage> clone(QWidget* parent = 0) const;
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;
  OptionalQString relationalTextFromRow(int row) const;
  OptionalInt selectedBankCode() const;

protected:
  void applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter);
  void applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder);

private:
  Q_DISABLE_COPY(BankListPage) 

  BankTableModel* model_;

private slots:
  void on_database_connectionStateChanged();
  void on_database_aboutToClose();      
  void on_database_bankInserted(const ma::chrono::model::Bank& bank);
  void on_database_bankUpdated(const ma::chrono::model::Bank& bank);
  void on_database_bankRemoved(const ma::chrono::model::Bank& bank);      
  void on_database_userUpdated(const ma::chrono::model::User& user);
}; // class BankListPage

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_BANKLISTPAGE_H

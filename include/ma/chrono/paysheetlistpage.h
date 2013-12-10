//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_PAYSHEETLISTPAGE_H
#define MA_CHRONO_PAYSHEETLISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/paysheettablemodel_fwd.h>
#include <ma/chrono/model/paysheet_fwd.h>
#include <ma/chrono/model/contract_fwd.h>
#include <ma/chrono/model/ccd_fwd.h>
#include <ma/chrono/model/currency_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/listpage.h>

namespace ma {
namespace chrono {

class PaysheetListPage : public ListPage
{
  Q_OBJECT

public:
  PaysheetListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalInt& filterId,
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent);
  ~PaysheetListPage();

  void refresh();
  std::auto_ptr<ListPage> clone(QWidget* parent = 0) const;      
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;      
  OptionalQString relationalTextFromRow(int row) const;

protected:
  void applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter);
  void applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder);

private:
  Q_DISABLE_COPY(PaysheetListPage)                      

  PaysheetTableModel* model_;
  OptionalInt filterId_;

private slots:
  void on_database_connectionStateChanged();
  void on_database_aboutToClose();      
  void on_database_paysheetInserted(
      const ma::chrono::model::Paysheet& paysheet);
  void on_database_paysheetUpdated(
      const ma::chrono::model::Paysheet& paysheet);
  void on_database_paysheetRemoved(
      const ma::chrono::model::Paysheet& paysheet);
  void on_database_contractUpdated(
      const ma::chrono::model::Contract& contract);
  void on_database_ccdUpdated(const ma::chrono::model::Ccd& ccd);
  void on_database_currencyUpdated(
      const ma::chrono::model::Currency& currency);
  void on_database_userUpdated(const ma::chrono::model::User& user);            
}; // class PaysheetListPage

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_PAYSHEETLISTPAGE_H

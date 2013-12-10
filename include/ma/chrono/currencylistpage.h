//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_CURRENCYLISTPAGE_H
#define MA_CHRONO_CURRENCYLISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/currencytablemodel_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/currency_fwd.h>
#include <ma/chrono/listpage.h>

namespace ma {
namespace chrono {

class CurrencyListPage : public ListPage
{
  Q_OBJECT

public:
  CurrencyListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent);
  ~CurrencyListPage();

  void refresh();
  std::auto_ptr<ListPage> clone(QWidget* parent = 0) const;
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;

protected:
  void applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter);
  void applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder);

private:
  Q_DISABLE_COPY(CurrencyListPage) 

  CurrencyTableModel* model_;

private slots:
  void on_database_connectionStateChanged();
  void on_database_aboutToClose();      
  void on_database_userUpdated(const ma::chrono::model::User& user);      
  void on_database_currencyInserted(
      const ma::chrono::model::Currency& currency);
  void on_database_currencyUpdated(
      const ma::chrono::model::Currency& currency);
  void on_database_currencyRemoved(
      const ma::chrono::model::Currency& currency);
}; // class CurrencyListPage

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_CURRENCYLISTPAGE_H

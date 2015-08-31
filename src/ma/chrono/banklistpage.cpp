/*
TRANSLATOR ma::chrono::BankListPage
*/

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <QAction>
#include <QTableView>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/banktablemodel.h>
#include <ma/chrono/bankcardpage.h>
#include <ma/chrono/model/bank.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/banklistpage.h>

namespace ma {
namespace chrono {

BankListPage::BankListPage(const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerWeakPtr& taskWindowManager,
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction,       
    const UserDefinedFilter& userDefinedFilter, 
    const UserDefinedOrder& userDefinedOrder,
    QWidget* parent)
  : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, 
        bankEntity, BankTableModel::tableDescription(), userDefinedFilter, 
        userDefinedOrder, parent)
  , model_(new BankTableModel(databaseModel, this, 
        userDefinedFilter.getUserDefinedFilterExpression(),
        userDefinedOrder.getUserDefinedOrderExpression()))
{       
  setWindowIcon(resourceManager->getIcon(chronoIconName));
  setWindowTitle(tr("Banks"));
  createAction()->setText(tr("&New bank"));
  setModel(model_);

  QObject::connect(databaseModel.get(), 
      SIGNAL(connectionStateChanged()), 
      SLOT(on_database_connectionStateChanged()));
  QObject::connect(databaseModel.get(), 
      SIGNAL(aboutToClose()), SLOT(on_database_aboutToClose()));
  QObject::connect(databaseModel.get(), 
      SIGNAL(bankInserted(const ma::chrono::model::Bank&)), 
      SLOT(on_database_bankInserted(const ma::chrono::model::Bank&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(bankUpdated(const ma::chrono::model::Bank&)), 
      SLOT(on_database_bankUpdated(const ma::chrono::model::Bank&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(bankRemoved(const ma::chrono::model::Bank&)), 
      SLOT(on_database_bankRemoved(const ma::chrono::model::Bank&)));      
  QObject::connect(databaseModel.get(), 
      SIGNAL(userUpdated(const ma::chrono::model::User&)), 
      SLOT(on_database_userUpdated(const ma::chrono::model::User&)));                        

  on_database_connectionStateChanged();
}

BankListPage::~BankListPage()
{
}

void BankListPage::refresh()
{
  model_->refresh();
}
        
std::auto_ptr<ListPage> BankListPage::clone(QWidget* parent) const
{
  std::auto_ptr<ListPage> clonedPage(new BankListPage(resourceManager(), 
      taskWindowManager(), databaseModel(), helpAction(), userDefinedFilter(),
      userDefinedOrder(), parent));
  clonedPage->setWindowTitle(windowTitle());            
  return clonedPage;
}

std::auto_ptr<CardPage> BankListPage::createCardPage(CardPage::Mode mode, 
    const OptionalQInt64& entityId) const
{
  std::auto_ptr<CardPage> cardPage(new BankCardPage(resourceManager(), 
      taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
  return cardPage;
} 

void BankListPage::applyUserDefinedFilter(
    const UserDefinedFilter& userDefinedFilter)
{
  model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
}

void BankListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
{
  model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
}

OptionalQString BankListPage::relationalTextFromRow(int row) const
{
  QVariant var = model_->data(model_->index(row, 3));
  if (!var.isNull())
  {          
    if (var.canConvert<QString>())
    {
      return var.toString();
    }        
  }
  return OptionalQString();
}

OptionalInt BankListPage::selectedBankCode() const
{
  int row = tableView()->currentIndex().row();
  if (row >= 0)
  {
    return model_->data(model_->index(row, 1)).toInt();        
  }
  return OptionalInt();
} 

void BankListPage::on_database_connectionStateChanged()
{
  if (databaseModel()->isOpen())
  {
    model_->open();
  }
}

void BankListPage::on_database_aboutToClose()
{
  model_->close();
}

void BankListPage::on_database_bankInserted(
    const ma::chrono::model::Bank& /*bank*/)
{
  model_->refresh();
}

void BankListPage::on_database_bankUpdated(
    const ma::chrono::model::Bank& /*bank*/)
{
  model_->refresh();
}

void BankListPage::on_database_bankRemoved(
    const ma::chrono::model::Bank& /*bank*/)
{
  model_->refresh();
}    

void BankListPage::on_database_userUpdated(
    const ma::chrono::model::User& /*user*/)
{
  model_->refresh();
}

} // namespace chrono
} // namespace ma

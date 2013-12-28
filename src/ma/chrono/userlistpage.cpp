/*
TRANSLATOR ma::chrono::UserListPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <QAction>
#include <QTableView>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/usertablemodel.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/usercardpage.h>
#include <ma/chrono/userlistpage.h>

namespace ma {

namespace chrono {    

UserListPage::UserListPage(const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerWeakPtr& taskWindowManager,
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction,       
    const UserDefinedFilter& userDefinedFilter, 
    const UserDefinedOrder& userDefinedOrder,
    QWidget* parent)
  : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction,
        userEntity, UserTableModel::tableDescription(), userDefinedFilter, 
        userDefinedOrder, parent)
  , model_(new UserTableModel(databaseModel, this, 
        userDefinedFilter.getUserDefinedFilterExpression(),
        userDefinedOrder.getUserDefinedOrderExpression()))
{    
  setWindowIcon(resourceManager->getIcon(chronoIconName));
  setWindowTitle(tr("Users"));            
  createAction()->setText(tr("&New user"));    
  setModel(model_);

  QObject::connect(databaseModel.get(), 
      SIGNAL(connectionStateChanged()), 
      SLOT(on_database_connectionStateChanged()));
  QObject::connect(databaseModel.get(), 
      SIGNAL(aboutToClose()), SLOT(on_database_aboutToClose()));
  QObject::connect(databaseModel.get(), 
      SIGNAL(userInserted(const ma::chrono::model::User&)), 
      SLOT(on_database_userInserted(const ma::chrono::model::User&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(userUpdated(const ma::chrono::model::User&)), 
      SLOT(on_database_userUpdated(const ma::chrono::model::User&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(userRemoved(const ma::chrono::model::User&)), 
      SLOT(on_database_userRemoved(const ma::chrono::model::User&)));

  on_database_connectionStateChanged();
}

UserListPage::~UserListPage()
{
}

void UserListPage::refresh()
{
  model_->refresh();      
}
        
std::auto_ptr<ListPage> UserListPage::clone(QWidget* parent) const
{
  std::auto_ptr<ListPage> clonedPage(new UserListPage(resourceManager(), 
      taskWindowManager(), databaseModel(), helpAction(), userDefinedFilter(), 
      userDefinedOrder(), parent));
  clonedPage->setWindowTitle(windowTitle());            
  return clonedPage;
}

std::auto_ptr<CardPage> UserListPage::createCardPage(CardPage::Mode mode,
    const OptionalQInt64& entityId) const
{      
  std::auto_ptr<CardPage> cardPage(new UserCardPage(
      resourceManager(), taskWindowManager(), databaseModel(), helpAction(), 
      mode, entityId));
  return cardPage;
}

void UserListPage::applyUserDefinedFilter(
    const UserDefinedFilter& userDefinedFilter)
{
  model_->setFilterExpression(
      userDefinedFilter.getUserDefinedFilterExpression());
}

void UserListPage::applyUserDefinedOrder(
    const UserDefinedOrder& userDefinedOrder)
{
  model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
}

void UserListPage::on_database_connectionStateChanged()
{
  if (databaseModel()->isOpen())
  {
    model_->open();
  }
}

void UserListPage::on_database_aboutToClose()
{
  model_->close();
}

void UserListPage::on_database_userInserted(
    const ma::chrono::model::User& /*user*/)
{
  model_->refresh();
}

void UserListPage::on_database_userUpdated(
    const ma::chrono::model::User& /*user*/)
{
  model_->refresh();
}

void UserListPage::on_database_userRemoved(
    const ma::chrono::model::User& /*user*/)
{
  model_->refresh();
}

} // namespace chrono
} //namespace ma

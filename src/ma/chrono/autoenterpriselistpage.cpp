/*
TRANSLATOR ma::chrono::AutoenterpriseListPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <QAction>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/autoenterprisetablemodel.h>
#include <ma/chrono/autoenterprisecardpage.h>
#include <ma/chrono/autoenterpriselistpage.h>

namespace ma {
namespace chrono {

AutoenterpriseListPage::AutoenterpriseListPage(
    const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerWeakPtr& taskWindowManager,
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction,       
    const UserDefinedFilter& userDefinedFilter, 
    const UserDefinedOrder& userDefinedOrder,
    QWidget* parent)
  : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction,
        autoenterpriseEntity, AutoenterpriseTableModel::tableDescription(), 
        userDefinedFilter, userDefinedOrder, parent)
  , model_(new AutoenterpriseTableModel(databaseModel, this, 
        userDefinedFilter.getUserDefinedFilterExpression(), 
        userDefinedOrder.getUserDefinedOrderExpression()))
{      
  setWindowIcon(resourceManager->getIcon(chronoIconName));
  setWindowTitle(tr("Autoenterprises"));
  createAction()->setText(tr("&New autoenterprise"));            
  setModel(model_);     

  QObject::connect(databaseModel.get(), 
      SIGNAL(connectionStateChanged()), 
      SLOT(on_database_connectionStateChanged()));
  QObject::connect(databaseModel.get(), 
      SIGNAL(aboutToClose()), 
      SLOT(on_database_aboutToClose()));
  QObject::connect(databaseModel.get(), 
      SIGNAL(autoenterpriseInserted(const ma::chrono::model::Autoenterprise&)),
      SLOT(on_database_autoenterpriseInserted(const ma::chrono::model::Autoenterprise&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(autoenterpriseUpdated(const ma::chrono::model::Autoenterprise&)), 
      SLOT(on_database_autoenterpriseUpdated(const ma::chrono::model::Autoenterprise&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(autoenterpriseRemoved(const ma::chrono::model::Autoenterprise&)), 
      SLOT(on_database_autoenterpriseRemoved(const ma::chrono::model::Autoenterprise&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(userUpdated(const ma::chrono::model::User&)), 
      SLOT(on_database_userUpdated(const ma::chrono::model::User&)));

  on_database_connectionStateChanged();
}

AutoenterpriseListPage::~AutoenterpriseListPage()
{
}

void AutoenterpriseListPage::refresh()
{
  model_->refresh();      
}
        
std::auto_ptr<ListPage> AutoenterpriseListPage::clone(QWidget* parent) const
{
  std::auto_ptr<ListPage> clonedPage(new AutoenterpriseListPage(
      resourceManager(), taskWindowManager(), databaseModel(), helpAction(),
      userDefinedFilter(), userDefinedOrder(), parent));
  clonedPage->setWindowTitle(windowTitle());
  return clonedPage;
}

std::auto_ptr<CardPage> AutoenterpriseListPage::createCardPage(
    CardPage::Mode mode, const OptionalQInt64& entityId) const
{
  std::auto_ptr<CardPage> cardPage(new AutoenterpriseCardPage(
      resourceManager(), taskWindowManager(), databaseModel(), 
      helpAction(), mode, entityId));
  return cardPage;
}

void AutoenterpriseListPage::applyUserDefinedFilter(
    const UserDefinedFilter& userDefinedFilter)
{
  model_->setFilterExpression(
      userDefinedFilter.getUserDefinedFilterExpression());
}

void AutoenterpriseListPage::applyUserDefinedOrder(
    const UserDefinedOrder& userDefinedOrder)
{
  model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
}

OptionalQString AutoenterpriseListPage::relationalTextFromRow(int row) const
{
  return buildOptionalQString(model_->data(model_->index(row, 2)));  
}

void AutoenterpriseListPage::on_database_connectionStateChanged()
{
  if (databaseModel()->isOpen())
  {
    model_->open();
  }
}

void AutoenterpriseListPage::on_database_aboutToClose()
{
  model_->close();
}

void AutoenterpriseListPage::on_database_autoenterpriseInserted(
    const ma::chrono::model::Autoenterprise& /*autoenterprise*/)
{
  model_->refresh();
}

void AutoenterpriseListPage::on_database_autoenterpriseUpdated(
    const ma::chrono::model::Autoenterprise& /*autoenterprise*/)
{
  model_->refresh();
}

void AutoenterpriseListPage::on_database_autoenterpriseRemoved(
    const ma::chrono::model::Autoenterprise& /*autoenterprise*/)
{
  model_->refresh();
}    

void AutoenterpriseListPage::on_database_userUpdated(
    const ma::chrono::model::User& /*user*/)
{
  model_->refresh();
}

} // namespace chrono
} //namespace ma

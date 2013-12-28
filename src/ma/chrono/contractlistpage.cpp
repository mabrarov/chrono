/*
TRANSLATOR ma::chrono::ContractListPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <QAction>
#include <QTableView>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/contracttablemodel.h>
#include <ma/chrono/contractcardpage.h>
#include <ma/chrono/amountdelegate.h>
#include <ma/chrono/model/bank.h>
#include <ma/chrono/model/resident.h>
#include <ma/chrono/model/counterpart.h>
#include <ma/chrono/model/currency.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/contractlistpage.h>

namespace ma
{
  namespace chrono
  {    
    ContractListPage::ContractListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalInt& filterId,
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, contractEntity, 
          ContractTableModel::tableDescription(), userDefinedFilter, userDefinedOrder, parent)
      , model_(new ContractTableModel(databaseModel, this, filterId, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
      , filterId_(filterId)
    {    
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Contracts"));
      createAction()->setText(tr("&New contract"));      
      setModel(model_);
      AmountDelegate* amountDelegate = new AmountDelegate(this);
      tableView()->setItemDelegateForColumn(7, amountDelegate);

      QObject::connect(databaseModel.get(), SIGNAL(connectionStateChanged()), SLOT(on_database_connectionStateChanged()));
      QObject::connect(databaseModel.get(), SIGNAL(aboutToClose()), SLOT(on_database_aboutToClose()));      
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));      
      QObject::connect(databaseModel.get(), SIGNAL(bankUpdated(const ma::chrono::model::Bank&)), SLOT(on_database_bankUpdated(const ma::chrono::model::Bank&)));      
      QObject::connect(databaseModel.get(), SIGNAL(residentUpdated(const ma::chrono::model::Resident&)), SLOT(on_database_residentUpdated(const ma::chrono::model::Resident&)));      
      QObject::connect(databaseModel.get(), SIGNAL(counterpartUpdated(const ma::chrono::model::Counterpart&)), SLOT(on_database_counterpartUpdated(const ma::chrono::model::Counterpart&)));      
      QObject::connect(databaseModel.get(), SIGNAL(currencyUpdated(const ma::chrono::model::Currency&)), SLOT(on_database_currencyUpdated(const ma::chrono::model::Currency&)));      
      QObject::connect(databaseModel.get(), SIGNAL(contractInserted(const ma::chrono::model::Contract&)), SLOT(on_database_contractInserted(const ma::chrono::model::Contract&)));
      QObject::connect(databaseModel.get(), SIGNAL(contractUpdated(const ma::chrono::model::Contract&)), SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));
      QObject::connect(databaseModel.get(), SIGNAL(contractRemoved(const ma::chrono::model::Contract&)), SLOT(on_database_contractRemoved(const ma::chrono::model::Contract&)));

      on_database_connectionStateChanged();
    }

    ContractListPage::~ContractListPage()
    {
    }

    void ContractListPage::refresh()
    {
      model_->refresh();      
    }
        
    std::auto_ptr<ListPage> ContractListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new ContractListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), filterId_, userDefinedFilter(), userDefinedOrder(), parent));
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> ContractListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {      
      std::auto_ptr<CardPage> cardPage(new ContractCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }        

    void ContractListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void ContractListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    void ContractListPage::on_database_connectionStateChanged()
    {
      if (databaseModel()->isOpen())
      {
        model_->open();
      }
    }

    void ContractListPage::on_database_aboutToClose()
    {
      model_->close();
    }    

    void ContractListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      model_->refresh();      
    }    

    void ContractListPage::on_database_bankUpdated(const ma::chrono::model::Bank& /*bank*/)
    {
      model_->refresh();
    }    

    void ContractListPage::on_database_residentUpdated(const ma::chrono::model::Resident& /*resident*/)
    {
      model_->refresh();
    }    

    void ContractListPage::on_database_counterpartUpdated(const ma::chrono::model::Counterpart& /*counterpart*/)
    {
      model_->refresh();
    }    

    void ContractListPage::on_database_currencyUpdated(const ma::chrono::model::Currency& /*currency*/)
    {
      model_->refresh();
    }    

    void ContractListPage::on_database_contractInserted(const ma::chrono::model::Contract& /*contract*/)
    {
      model_->refresh();
    }

    void ContractListPage::on_database_contractUpdated(const ma::chrono::model::Contract& /*contract*/)
    {
      model_->refresh();
    }

    void ContractListPage::on_database_contractRemoved(const ma::chrono::model::Contract& /*contract*/)
    {
      model_->refresh();
    }

  } // namespace chrono
} //namespace ma
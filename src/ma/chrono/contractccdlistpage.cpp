/*
TRANSLATOR ma::chrono::ContractCcdListPage
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <QAction>
#include <QTableView>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/contractccdtablemodel.h>
#include <ma/chrono/ccdcardpage.h>
#include <ma/chrono/amountdelegate.h>
#include <ma/chrono/ratedelegate.h>
#include <ma/chrono/model/ccd.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/counterpart.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/deppoint.h>
#include <ma/chrono/model/bank.h>
#include <ma/chrono/model/tterm.h>
#include <ma/chrono/model/currency.h>
#include <ma/chrono/contractccdlistpage.h>

namespace ma
{
  namespace chrono
  {    
    ContractCcdListPage::ContractCcdListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction,
      const OptionalQInt64& contractId, 
      const UserDefinedFilter& userDefinedFilter,
      const UserDefinedOrder& userDefinedOrder,      
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, ccdEntity, ContractCcdTableModel::tableDescription(), 
          userDefinedFilter, userDefinedOrder, parent)      
      , contractId_(contractId)
      , model_(new ContractCcdTableModel(databaseModel, this, contractId, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
    {    
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("CCDs"));
      createAction()->setText(tr("&New ccd"));

      setModel(model_);
      AmountDelegate* amountDelegate = new AmountDelegate(this);
      tableView()->setItemDelegateForColumn(2, amountDelegate);
      AmountDelegate* dutyDelegate = new AmountDelegate(this);
      tableView()->setItemDelegateForColumn(6, dutyDelegate);
      RateDelegate* rateDelegate = new RateDelegate(this);
      tableView()->setItemDelegateForColumn(7, rateDelegate);
      
      actions_ << viewAction();
      selectedRowActions_ << viewAction();      
      actions_ << editAction();
      selectedRowActions_ << editAction();
      actions_ << createAction();
      actions_ << deleteAction();
      selectedRowActions_ << deleteAction();
      QAction* separator = new QAction(this);
      separator->setSeparator(true);
      actions_ << separator;
      actions_ << openInNewWindowAction();
      actions_ << refreshAction();
      actions_ << sendToExcelAction();      

      QObject::connect(databaseModel.get(), SIGNAL(connectionStateChanged()), SLOT(on_database_connectionStateChanged()));
      QObject::connect(databaseModel.get(), SIGNAL(aboutToClose()), SLOT(on_database_aboutToClose()));      
      QObject::connect(databaseModel.get(), SIGNAL(ccdInserted(const ma::chrono::model::Ccd&)), SLOT(on_database_ccdInserted(const ma::chrono::model::Ccd&)));
      QObject::connect(databaseModel.get(), SIGNAL(ccdUpdated(const ma::chrono::model::Ccd&)), SLOT(on_database_ccdUpdated(const ma::chrono::model::Ccd&)));
      QObject::connect(databaseModel.get(), SIGNAL(ccdRemoved(const ma::chrono::model::Ccd&)), SLOT(on_database_ccdRemoved(const ma::chrono::model::Ccd&)));
      QObject::connect(databaseModel.get(), SIGNAL(contractUpdated(const ma::chrono::model::Contract&)), SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));      
      QObject::connect(databaseModel.get(), SIGNAL(contractRemoved(const ma::chrono::model::Contract&)), SLOT(on_database_contractRemoved(const ma::chrono::model::Contract&)));      
      QObject::connect(databaseModel.get(), SIGNAL(counterpartUpdated(const ma::chrono::model::Counterpart&)), SLOT(on_database_counterpartUpdated(const ma::chrono::model::Counterpart&)));
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));                  
      QObject::connect(databaseModel.get(), SIGNAL(bankUpdated(const ma::chrono::model::Bank&)), SLOT(on_database_bankUpdated(const ma::chrono::model::Bank&)));
      QObject::connect(databaseModel.get(), SIGNAL(ttermUpdated(const ma::chrono::model::Tterm&)), SLOT(on_database_ttermUpdated(const ma::chrono::model::Tterm&)));      
      QObject::connect(databaseModel.get(), SIGNAL(currencyUpdated(const ma::chrono::model::Currency&)), SLOT(on_database_currencyUpdated(const ma::chrono::model::Currency&)));            

      on_database_connectionStateChanged();
    }

    ContractCcdListPage::~ContractCcdListPage()
    {
    }

    void ContractCcdListPage::refresh()
    {
      model_->refresh();
    }
        
    std::auto_ptr<ListPage> ContractCcdListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new ContractCcdListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), contractId_, userDefinedFilter(), userDefinedOrder(), parent));      
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> ContractCcdListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CcdCardPage> cardPage(new CcdCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      if (CardPage::createMode == mode && contractId_)
      {
        cardPage->setSelectedContractId(contractId_.get());
      }
      return cardPage;
    }

    void ContractCcdListPage::setContractId(qint64 contractId)
    {
      if (!contractId_)
      {
        contractId_ = contractId;
        model_->setContractId(contractId);
        on_database_connectionStateChanged();
      }
    }

    QActionList ContractCcdListPage::parentActions() const
    {
      return actions_;
    }

    QActionList ContractCcdListPage::selectedParentActions() const
    {
      return selectedRowActions_;
    }

    void ContractCcdListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void ContractCcdListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    void ContractCcdListPage::updateContractRelatedData()
    {
      static const char* querySql = "select " \
        "con.\"NUMBER\" " \
        "from \"CONTRACT\" con " \
        "where con.\"CONTRACT_ID\" = ?";

      if (contractId_)
      {
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {          
          query.bindValue(0, contractId_.get());
          if (query.exec())
          {
            if (query.next())
            {
              QString contractNumber = query.value(0).toString();
              updateWindowTitle(tr("Contracts - %1 - CCDs").arg(contractNumber));
            }
          }
        }
      }      
    }

    void ContractCcdListPage::on_database_connectionStateChanged()
    {
      if (contractId_ && databaseModel()->isOpen())
      {
        model_->open();
        updateContractRelatedData();
      }
    }

    void ContractCcdListPage::on_database_aboutToClose()
    {
      if (contractId_)
      {
        model_->close();
      }
    }

    void ContractCcdListPage::on_database_ccdInserted(const ma::chrono::model::Ccd& /*ccd*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }

    void ContractCcdListPage::on_database_ccdUpdated(const ma::chrono::model::Ccd& /*ccd*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }

    void ContractCcdListPage::on_database_ccdRemoved(const ma::chrono::model::Ccd& /*ccd*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }

    void ContractCcdListPage::on_database_contractUpdated(const ma::chrono::model::Contract& contract)
    {
      if (contractId_ && contractId_.get() == contract.contractId)
      {
        updateContractRelatedData();
      }      
    }    

    void ContractCcdListPage::on_database_contractRemoved(const ma::chrono::model::Contract& contract)
    {
      if (contractId_ && contractId_.get() == contract.contractId)
      {
        emit needToClose();
      }      
    }

    void ContractCcdListPage::on_database_counterpartUpdated(const ma::chrono::model::Counterpart& /*counterpart*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }

    void ContractCcdListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }

    void ContractCcdListPage::on_database_deppointUpdated(const ma::chrono::model::Deppoint& /*deppoint*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }

    void ContractCcdListPage::on_database_bankUpdated(const ma::chrono::model::Bank& /*bank*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }

    void ContractCcdListPage::on_database_ttermUpdated(const ma::chrono::model::Tterm& /*tterm*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }
    
    void ContractCcdListPage::on_database_currencyUpdated(const ma::chrono::model::Currency& /*currency*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }

  } // namespace chrono
} // namespace ma
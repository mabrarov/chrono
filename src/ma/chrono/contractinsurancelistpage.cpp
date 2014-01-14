/*
TRANSLATOR ma::chrono::ContractInsuranceListPage
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
#include <ma/chrono/contractinsurancetablemodel.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/insurance.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/currency.h>
#include <ma/chrono/insurancecardpage.h>
#include <ma/chrono/amountdelegate.h>
#include <ma/chrono/contractinsurancelistpage.h>

namespace ma
{
  namespace chrono
  {    
    ContractInsuranceListPage::ContractInsuranceListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction,
      const OptionalQInt64& contractId, 
      const UserDefinedFilter& userDefinedFilter,
      const UserDefinedOrder& userDefinedOrder,      
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, insuranceEntity, ContractInsuranceTableModel::tableDescription(), 
          userDefinedFilter, userDefinedOrder, parent)      
      , contractId_(contractId)
      , model_(new ContractInsuranceTableModel(databaseModel, this, contractId, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
    {    
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Inurances"));
      createAction()->setText(tr("&New insurance"));

      setModel(model_);
      AmountDelegate* amountDelegate = new AmountDelegate(this);
      tableView()->setItemDelegateForColumn(2, amountDelegate);
      
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
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));      
      QObject::connect(databaseModel.get(), SIGNAL(contractUpdated(const ma::chrono::model::Contract&)), SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));      
      QObject::connect(databaseModel.get(), SIGNAL(contractRemoved(const ma::chrono::model::Contract&)), SLOT(on_database_contractRemoved(const ma::chrono::model::Contract&)));      
      QObject::connect(databaseModel.get(), SIGNAL(currencyUpdated(const ma::chrono::model::Currency&)), SLOT(on_database_currencyUpdated(const ma::chrono::model::Currency&)));      
      QObject::connect(databaseModel.get(), SIGNAL(insuranceInserted(const ma::chrono::model::Insurance&)), SLOT(on_database_insuranceInserted(const ma::chrono::model::Insurance&)));
      QObject::connect(databaseModel.get(), SIGNAL(insuranceUpdated(const ma::chrono::model::Insurance&)), SLOT(on_database_insuranceUpdated(const ma::chrono::model::Insurance&)));
      QObject::connect(databaseModel.get(), SIGNAL(insuranceRemoved(const ma::chrono::model::Insurance&)), SLOT(on_database_insuranceRemoved(const ma::chrono::model::Insurance&)));

      on_database_connectionStateChanged();
    }

    ContractInsuranceListPage::~ContractInsuranceListPage()
    {
    }

    void ContractInsuranceListPage::refresh()
    {
      model_->refresh();
    }
        
    std::auto_ptr<ListPage> ContractInsuranceListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new ContractInsuranceListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), contractId_, userDefinedFilter(), userDefinedOrder(), parent));      
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> ContractInsuranceListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<InsuranceCardPage> cardPage(new InsuranceCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      if (CardPage::createMode == mode && contractId_)
      {
        cardPage->setSelectedContractId(contractId_.get());
      }
      return cardPage;
    }

    void ContractInsuranceListPage::setContractId(qint64 contractId)
    {
      contractId_ = contractId;
      model_->setContractId(contractId);      
      on_database_connectionStateChanged();      
    }

    void ContractInsuranceListPage::updateContractRelatedData()
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
              updateWindowTitle(tr("Contracts - %1 - Insurances").arg(contractNumber));
            }
          }
        }
      }      
    }

    QActionList ContractInsuranceListPage::parentActions() const
    {
      return actions_;
    }

    QActionList ContractInsuranceListPage::selectedParentActions() const
    {
      return selectedRowActions_;
    }

    void ContractInsuranceListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void ContractInsuranceListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    void ContractInsuranceListPage::on_database_connectionStateChanged()
    {
      if (contractId_ && databaseModel()->isOpen())
      {
        model_->open();
        updateContractRelatedData();
      }
    }

    void ContractInsuranceListPage::on_database_aboutToClose()
    {
      if (contractId_)
      {
        model_->close();
      }
    }

    void ContractInsuranceListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      model_->refresh();
    }    

    void ContractInsuranceListPage::on_database_contractUpdated(const ma::chrono::model::Contract& contract)
    {
      if (contractId_ && contractId_.get() == contract.contractId)
      {
        updateContractRelatedData();
      }      
    }    

    void ContractInsuranceListPage::on_database_contractRemoved(const ma::chrono::model::Contract& contract)
    {
      if (contractId_ && contractId_.get() == contract.contractId)
      {
        emit needToClose();
      }      
    }

    void ContractInsuranceListPage::on_database_currencyUpdated(const ma::chrono::model::Currency& /*currency*/)
    {
      model_->refresh();
    }    

    void ContractInsuranceListPage::on_database_insuranceInserted(const ma::chrono::model::Insurance& /*insurance*/)
    {
      model_->refresh();
    }

    void ContractInsuranceListPage::on_database_insuranceUpdated(const ma::chrono::model::Insurance& /*insurance*/)
    {
      model_->refresh();
    }

    void ContractInsuranceListPage::on_database_insuranceRemoved(const ma::chrono::model::Insurance& /*insurance*/)
    {
      model_->refresh();
    }

  } // namespace chrono
} // namespace ma
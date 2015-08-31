/*
TRANSLATOR ma::chrono::ContractSbolListPage
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
#include <ma/chrono/contractsboltablemodel.h>
#include <ma/chrono/sbolcardpage.h>
#include <ma/chrono/model/sbol.h>
#include <ma/chrono/model/ccd.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/lcredit.h>
#include <ma/chrono/contractsbollistpage.h>

namespace ma
{
  namespace chrono
  {    
    ContractSbolListPage::ContractSbolListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction,
      const OptionalQInt64& contractId, 
      const UserDefinedFilter& userDefinedFilter,
      const UserDefinedOrder& userDefinedOrder,      
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, sbolEntity, ContractSbolTableModel::tableDescription(), 
          userDefinedFilter, userDefinedOrder, parent)      
      , contractId_(contractId)
      , model_(new ContractSbolTableModel(databaseModel, this, contractId, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
    {    
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Shipped bills of lading"));
      createAction()->setText(tr("&New shipped bill of lading"));

      setModel(model_);      
      
      actions_ << viewAction();
      selectedRowActions_ << viewAction();      
      actions_ << editAction();
      selectedRowActions_ << editAction();      
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
      QObject::connect(databaseModel.get(), SIGNAL(contractUpdated(const ma::chrono::model::Contract&)), SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));      
      QObject::connect(databaseModel.get(), SIGNAL(contractRemoved(const ma::chrono::model::Contract&)), SLOT(on_database_contractRemoved(const ma::chrono::model::Contract&)));      
      QObject::connect(databaseModel.get(), SIGNAL(sbolInserted(const ma::chrono::model::Sbol&)), SLOT(on_database_sbolInserted(const ma::chrono::model::Sbol&)));
      QObject::connect(databaseModel.get(), SIGNAL(sbolUpdated(const ma::chrono::model::Sbol&)), SLOT(on_database_sbolUpdated(const ma::chrono::model::Sbol&)));
      QObject::connect(databaseModel.get(), SIGNAL(sbolRemoved(const ma::chrono::model::Sbol&)), SLOT(on_database_sbolRemoved(const ma::chrono::model::Sbol&)));
      QObject::connect(databaseModel.get(), SIGNAL(ccdUpdated(const ma::chrono::model::Ccd&)), SLOT(on_database_ccdUpdated(const ma::chrono::model::Ccd&)));      
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));                        
      QObject::connect(databaseModel.get(), SIGNAL(lcreditUpdated(const ma::chrono::model::Lcredit&)), SLOT(on_database_lcreditUpdated(const ma::chrono::model::Lcredit&)));

      on_database_connectionStateChanged();
    }

    ContractSbolListPage::~ContractSbolListPage()
    {
    }

    void ContractSbolListPage::refresh()
    {
      model_->refresh();
    }
        
    std::auto_ptr<ListPage> ContractSbolListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new ContractSbolListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), contractId_, userDefinedFilter(), userDefinedOrder(), parent));      
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> ContractSbolListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<SbolCardPage> cardPage(new SbolCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));      
      return cardPage;
    }

    void ContractSbolListPage::setContractId(qint64 contractId)
    {
      if (!contractId_)
      {
        contractId_ = contractId;
        model_->setContractId(contractId);
        on_database_connectionStateChanged();
      }
    }

    QActionList ContractSbolListPage::parentActions() const
    {
      return actions_;
    }

    QActionList ContractSbolListPage::selectedParentActions() const
    {
      return selectedRowActions_;
    }

    void ContractSbolListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void ContractSbolListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    void ContractSbolListPage::updateContractRelatedData()
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
              updateWindowTitle(tr("Contracts - %1 - SBOL").arg(contractNumber));
            }
          }
        }
      }      
    }

    void ContractSbolListPage::on_database_connectionStateChanged()
    {
      if (contractId_ && databaseModel()->isOpen())
      {
        model_->open();
        updateContractRelatedData();
      }
    }

    void ContractSbolListPage::on_database_aboutToClose()
    {
      if (contractId_)
      {
        model_->close();
      }
    }    

    void ContractSbolListPage::on_database_contractUpdated(const ma::chrono::model::Contract& contract)
    {
      if (contractId_ && contractId_.get() == contract.contractId)
      {
        updateContractRelatedData();
      }      
    }    

    void ContractSbolListPage::on_database_contractRemoved(const ma::chrono::model::Contract& contract)
    {
      if (contractId_ && contractId_.get() == contract.contractId)
      {
        emit needToClose();
      }      
    }

    void ContractSbolListPage::on_database_sbolInserted(const ma::chrono::model::Sbol& /*sbol*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }

    void ContractSbolListPage::on_database_sbolUpdated(const ma::chrono::model::Sbol& /*sbol*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }

    void ContractSbolListPage::on_database_sbolRemoved(const ma::chrono::model::Sbol& /*sbol*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }

    void ContractSbolListPage::on_database_ccdUpdated(const ma::chrono::model::Ccd& /*ccd*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }        

    void ContractSbolListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }    

    void ContractSbolListPage::on_database_lcreditUpdated(const ma::chrono::model::Lcredit& /*lcredit*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }

  } // namespace chrono
} // namespace ma
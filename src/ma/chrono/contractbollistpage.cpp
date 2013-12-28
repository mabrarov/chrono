/*
TRANSLATOR ma::chrono::ContractBolListPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <QAction>
#include <QTableView>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/contractboltablemodel.h>
#include <ma/chrono/bolcardpage.h>
#include <ma/chrono/model/bol.h>
#include <ma/chrono/model/ccd.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/autoenterprise.h>
#include <ma/chrono/contractbollistpage.h>

namespace ma
{
  namespace chrono
  {    
    ContractBolListPage::ContractBolListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction,
      const OptionalQInt64& contractId, 
      const UserDefinedFilter& userDefinedFilter,
      const UserDefinedOrder& userDefinedOrder,      
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, bolEntity, ContractBolTableModel::tableDescription(), 
          userDefinedFilter, userDefinedOrder, parent)      
      , contractId_(contractId)
      , model_(new ContractBolTableModel(databaseModel, this, contractId, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
    {    
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Bills of lading"));
      createAction()->setText(tr("&New bill of lading"));

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
      QObject::connect(databaseModel.get(), SIGNAL(bolInserted(const ma::chrono::model::Bol&)), SLOT(on_database_bolInserted(const ma::chrono::model::Bol&)));
      QObject::connect(databaseModel.get(), SIGNAL(bolUpdated(const ma::chrono::model::Bol&)), SLOT(on_database_bolUpdated(const ma::chrono::model::Bol&)));
      QObject::connect(databaseModel.get(), SIGNAL(bolRemoved(const ma::chrono::model::Bol&)), SLOT(on_database_bolRemoved(const ma::chrono::model::Bol&)));
      QObject::connect(databaseModel.get(), SIGNAL(ccdUpdated(const ma::chrono::model::Ccd&)), SLOT(on_database_ccdUpdated(const ma::chrono::model::Ccd&)));      
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));                        
      QObject::connect(databaseModel.get(), SIGNAL(autoenterpriseUpdated(const ma::chrono::model::Autoenterprise&)), SLOT(on_database_autoenterpriseUpdated(const ma::chrono::model::Autoenterprise&)));

      on_database_connectionStateChanged();
    }

    ContractBolListPage::~ContractBolListPage()
    {
    }

    void ContractBolListPage::refresh()
    {
      model_->refresh();
    }
        
    std::auto_ptr<ListPage> ContractBolListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new ContractBolListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), contractId_, userDefinedFilter(), userDefinedOrder(), parent));      
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> ContractBolListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<BolCardPage> cardPage(new BolCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));      
      return cardPage;
    }

    void ContractBolListPage::setContractId(qint64 contractId)
    {
      if (!contractId_)
      {
        contractId_ = contractId;
        model_->setContractId(contractId);
        on_database_connectionStateChanged();
      }
    }

    QActionList ContractBolListPage::parentActions() const
    {
      return actions_;
    }

    QActionList ContractBolListPage::selectedParentActions() const
    {
      return selectedRowActions_;
    }

    void ContractBolListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void ContractBolListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    void ContractBolListPage::updateContractRelatedData()
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
              updateWindowTitle(tr("Contracts - %1 - BOL").arg(contractNumber));
            }
          }
        }
      }      
    }

    void ContractBolListPage::on_database_connectionStateChanged()
    {
      if (contractId_ && databaseModel()->isOpen())
      {
        model_->open();
        updateContractRelatedData();
      }
    }

    void ContractBolListPage::on_database_aboutToClose()
    {
      if (contractId_)
      {
        model_->close();
      }
    }    

    void ContractBolListPage::on_database_contractUpdated(const ma::chrono::model::Contract& contract)
    {
      if (contractId_ && contractId_.get() == contract.contractId)
      {
        updateContractRelatedData();
      }      
    }    

    void ContractBolListPage::on_database_contractRemoved(const ma::chrono::model::Contract& contract)
    {
      if (contractId_ && contractId_.get() == contract.contractId)
      {
        emit needToClose();
      }      
    }

    void ContractBolListPage::on_database_bolInserted(const ma::chrono::model::Bol& /*bol*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }

    void ContractBolListPage::on_database_bolUpdated(const ma::chrono::model::Bol& /*bol*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }

    void ContractBolListPage::on_database_bolRemoved(const ma::chrono::model::Bol& /*bol*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }

    void ContractBolListPage::on_database_ccdUpdated(const ma::chrono::model::Ccd& /*ccd*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }        

    void ContractBolListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }    

    void ContractBolListPage::on_database_autoenterpriseUpdated(const ma::chrono::model::Autoenterprise& /*autoenterprise*/)
    {
      if (contractId_)
      {
        model_->refresh();
      }      
    }

  } // namespace chrono
} //namespace ma
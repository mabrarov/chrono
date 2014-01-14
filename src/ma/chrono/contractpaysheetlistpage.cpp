/*
TRANSLATOR ma::chrono::ContractPaysheetListPage
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
#include <ma/chrono/contractpaysheettablemodel.h>
#include <ma/chrono/paysheetcardpage.h>
#include <ma/chrono/model/paysheet.h>
#include <ma/chrono/amountdelegate.h>
#include <ma/chrono/model/paysheet.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/ccd.h>
#include <ma/chrono/model/currency.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/contractpaysheetlistpage.h>

namespace ma
{
  namespace chrono
  {    
    ContractPaysheetListPage::ContractPaysheetListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction,
      const OptionalQInt64& contractId, 
      const UserDefinedFilter& userDefinedFilter,
      const UserDefinedOrder& userDefinedOrder,      
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, paysheetEntity, ContractPaysheetTableModel::tableDescription(), 
          userDefinedFilter, userDefinedOrder, parent)      
      , contractId_(contractId)
      , model_(new ContractPaysheetTableModel(databaseModel, this, contractId, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
    {    
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Pay sheets"));
      createAction()->setText(tr("&New pay sheet"));

      setModel(model_);
      AmountDelegate* amountDelegate = new AmountDelegate(this);
      tableView()->setItemDelegateForColumn(4, amountDelegate);
      
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
      QObject::connect(databaseModel.get(), SIGNAL(paysheetInserted(const ma::chrono::model::Paysheet&)), SLOT(on_database_paysheetInserted(const ma::chrono::model::Paysheet&)));
      QObject::connect(databaseModel.get(), SIGNAL(paysheetUpdated(const ma::chrono::model::Paysheet&)), SLOT(on_database_paysheetUpdated(const ma::chrono::model::Paysheet&)));      
      QObject::connect(databaseModel.get(), SIGNAL(paysheetRemoved(const ma::chrono::model::Paysheet&)), SLOT(on_database_paysheetRemoved(const ma::chrono::model::Paysheet&)));
      QObject::connect(databaseModel.get(), SIGNAL(contractUpdated(const ma::chrono::model::Contract&)), SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));
      QObject::connect(databaseModel.get(), SIGNAL(contractRemoved(const ma::chrono::model::Contract&)), SLOT(on_database_contractRemoved(const ma::chrono::model::Contract&)));      
      QObject::connect(databaseModel.get(), SIGNAL(ccdUpdated(const ma::chrono::model::Ccd&)), SLOT(on_database_ccdUpdated(const ma::chrono::model::Ccd&)));
      QObject::connect(databaseModel.get(), SIGNAL(currencyUpdated(const ma::chrono::model::Currency&)), SLOT(on_database_currencyUpdated(const ma::chrono::model::Currency&)));
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));                        

      on_database_connectionStateChanged();
    }

    ContractPaysheetListPage::~ContractPaysheetListPage()
    {
    }

    void ContractPaysheetListPage::refresh()
    {
      model_->refresh();
    }
        
    std::auto_ptr<ListPage> ContractPaysheetListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new ContractPaysheetListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), contractId_, userDefinedFilter(), userDefinedOrder(), parent));      
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> ContractPaysheetListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<PaysheetCardPage> cardPage(new PaysheetCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      if (CardPage::createMode == mode && contractId_)
      {
        cardPage->setSelectedContractId(contractId_.get());
      }
      return cardPage;
    }

    void ContractPaysheetListPage::setContractId(qint64 contractId)
    {
      if (!contractId_)
      {
        contractId_ = contractId;
        model_->setContractId(contractId);
        on_database_connectionStateChanged();
      }
    }

    QActionList ContractPaysheetListPage::parentActions() const
    {
      return actions_;
    }

    QActionList ContractPaysheetListPage::selectedParentActions() const
    {
      return selectedRowActions_;
    }

    void ContractPaysheetListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void ContractPaysheetListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    void ContractPaysheetListPage::updateContractRelatedData()
    {
      static const char* querySql = "select " \
        "t.\"NUMBER\", t.\"AMOUNT\", "
        "t.\"PASSPORT_NUMBER\", t.\"CLOSE_MARK\", " \
        "t.\"SIGNING_DATE\", t.\"END_DATE\", " \
        "t.\"REMARK\", t.\"CLOSE_REMARK\", " \
        "t.\"RESIDENT_ID\", r.\"SHORT_NAME\", " \
        "t.\"CURRENCY_ID\", cur.\"FULL_NAME\", " \
        "t.\"COUNTERPART_ID\", cntp.\"SHORT_NAME\", " \
        "t.\"BANK_ID\", b.\"SHORT_NAME\", " \
        "t.\"CREATE_USER_ID\", cu.\"LOGIN\", " \
        "t.\"UPDATE_USER_ID\", uu.\"LOGIN\", " \
        "t.\"CLOSE_USER_ID\", clu.\"LOGIN\", " \
        "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", t.\"CLOSE_TIME\", " \
        "t.\"PASSPORT_END_DATE\", t.\"CLOSE_DATE\", t.\"PASSPORT_SIGNING_DATE\" " \
        "from \"CONTRACT\" t " \
        "join \"RESIDENT\" r on t.\"RESIDENT_ID\" = r.\"RESIDENT_ID\""
        "join \"CURRENCY\" cur on t.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
        "join \"COUNTERPART\" cntp on t.\"COUNTERPART_ID\" = cntp.\"COUNTERPART_ID\" " \
        "join \"BANK\" b on t.\"BANK_ID\" = b.\"BANK_ID\" " \
        "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
        "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
        "left join \"USER\" clu on t.\"CLOSE_USER_ID\" = clu.\"USER_ID\" " \
        "where t.\"CONTRACT_ID\" = ?";

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
              model::Contract contract;
              contract.contractId = contractId_.get();
              contract.number     = query.value(0).toString();
              contract.amount     = query.value(1).toLongLong();
              contract.passportNumber = query.isNull(2) ? OptionalQString() : query.value(2).toString(); 
              contract.closeMark  = query.value(3).toInt() != 0;
              contract.signingDate = databaseModel()->convertFromServer(query.value(4).toDate());
              contract.endDate     = databaseModel()->convertFromServer(query.value(5).toDate());
              contract.remark      = query.isNull(6) ? OptionalQString() : query.value(6).toString();
              contract.closeRemark = query.isNull(7) ? OptionalQString() : query.value(7).toString();          
              contract.residentId  = query.value(8).toLongLong();              
              contract.currencyId  = query.value(10).toLongLong();              
              contract.counterpartId = query.value(12).toLongLong();              
              contract.bankId      = query.value(14).toLongLong();              
              contract.createUserId = query.value(16).toLongLong();              
              contract.updateUserId = query.value(18).toLongLong();              
              contract.closeUserId  = query.isNull(20) ? OptionalQInt64()  : query.value(20).toLongLong();              
              contract.createTime   = databaseModel()->convertFromServer(query.value(22).toDateTime()); 
              contract.updateTime   = databaseModel()->convertFromServer(query.value(23).toDateTime()); 
              contract.closeTime    = query.isNull(24) ? OptionalQDateTime() : databaseModel()->convertFromServer(query.value(24).toDateTime());              
              contract.passportEndDate = databaseModel()->convertFromServer(query.value(25).toDate()); 
              contract.closeDate    = query.isNull(26) ? OptionalQDate() : databaseModel()->convertFromServer(query.value(26).toDate()); 
              contract.passportSigningDate = databaseModel()->convertFromServer(query.value(27).toDate());               

              updateWindowTitle(tr("Contracts - %1 - Pay sheets").arg(contract.number));
            }
          }
        }
      }      
    }

    void ContractPaysheetListPage::on_database_connectionStateChanged()
    {
      if (contractId_ && databaseModel()->isOpen())
      {
        model_->open();
        updateContractRelatedData();
      }
    }

    void ContractPaysheetListPage::on_database_aboutToClose()
    {
      if (contractId_)
      {
        model_->close();
      }
    }

    void ContractPaysheetListPage::on_database_paysheetInserted(const ma::chrono::model::Paysheet& /*paysheet*/)
    {
      model_->refresh();
    }

    void ContractPaysheetListPage::on_database_paysheetUpdated(const ma::chrono::model::Paysheet& /*paysheet*/)
    {
      model_->refresh();
    }

    void ContractPaysheetListPage::on_database_paysheetRemoved(const ma::chrono::model::Paysheet& /*paysheet*/)
    {
      model_->refresh();
    }    

    void ContractPaysheetListPage::on_database_contractUpdated(const ma::chrono::model::Contract& contract)
    {
      if (contractId_ && contractId_.get() == contract.contractId)
      {
        updateContractRelatedData();
      }      
    }

    void ContractPaysheetListPage::on_database_contractRemoved(const ma::chrono::model::Contract& contract)
    {
      if (contractId_ && contractId_.get() == contract.contractId)
      {
        emit needToClose();
      }      
    }

    void ContractPaysheetListPage::on_database_ccdUpdated(const ma::chrono::model::Ccd& /*ccd*/)
    {
      model_->refresh();
    }    

    void ContractPaysheetListPage::on_database_currencyUpdated(const ma::chrono::model::Currency& /*Currency*/)
    {
      model_->refresh();
    }    

    void ContractPaysheetListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      model_->refresh();
    }            

  } // namespace chrono
} // namespace ma
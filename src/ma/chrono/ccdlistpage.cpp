/*
TRANSLATOR ma::chrono::CcdListPage
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
#include <ma/chrono/ccdtablemodel.h>
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
#include <ma/chrono/ccdlistpage.h>

namespace ma
{
  namespace chrono
  {    
    CcdListPage::CcdListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalInt& filterId,
      const UserDefinedFilter& userDefinedFilter,
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, ccdEntity, 
          CcdTableModel::tableDescription(), userDefinedFilter, userDefinedOrder, parent)
      , model_(new CcdTableModel(databaseModel, this, filterId,
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
      , filterId_(filterId)
    {    
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Ccds"));
      createAction()->setText(tr("&New ccd"));      
      setModel(model_);
      AmountDelegate* amountDelegate = new AmountDelegate(this);
      tableView()->setItemDelegateForColumn(2, amountDelegate);
      AmountDelegate* dutyDelegate = new AmountDelegate(this);
      tableView()->setItemDelegateForColumn(11, dutyDelegate);
      RateDelegate* rateDelegate = new RateDelegate(this);
      tableView()->setItemDelegateForColumn(12, rateDelegate);

      QObject::connect(databaseModel.get(), SIGNAL(connectionStateChanged()), SLOT(on_database_connectionStateChanged()));
      QObject::connect(databaseModel.get(), SIGNAL(aboutToClose()), SLOT(on_database_aboutToClose()));      
      QObject::connect(databaseModel.get(), SIGNAL(ccdInserted(const ma::chrono::model::Ccd&)), SLOT(on_database_ccdInserted(const ma::chrono::model::Ccd&)));
      QObject::connect(databaseModel.get(), SIGNAL(ccdUpdated(const ma::chrono::model::Ccd&)), SLOT(on_database_ccdUpdated(const ma::chrono::model::Ccd&)));
      QObject::connect(databaseModel.get(), SIGNAL(ccdRemoved(const ma::chrono::model::Ccd&)), SLOT(on_database_ccdRemoved(const ma::chrono::model::Ccd&)));
      QObject::connect(databaseModel.get(), SIGNAL(contractUpdated(const ma::chrono::model::Contract&)), SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));
      QObject::connect(databaseModel.get(), SIGNAL(counterpartUpdated(const ma::chrono::model::Counterpart&)), SLOT(on_database_counterpartUpdated(const ma::chrono::model::Counterpart&)));
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));                  
      QObject::connect(databaseModel.get(), SIGNAL(bankUpdated(const ma::chrono::model::Bank&)), SLOT(on_database_bankUpdated(const ma::chrono::model::Bank&)));
      QObject::connect(databaseModel.get(), SIGNAL(ttermUpdated(const ma::chrono::model::Tterm&)), SLOT(on_database_ttermUpdated(const ma::chrono::model::Tterm&)));      
      QObject::connect(databaseModel.get(), SIGNAL(currencyUpdated(const ma::chrono::model::Currency&)), SLOT(on_database_currencyUpdated(const ma::chrono::model::Currency&)));            

      on_database_connectionStateChanged();
    }

    CcdListPage::~CcdListPage()
    {
    }

    void CcdListPage::refresh()
    {
      model_->refresh();
    }
        
    std::auto_ptr<ListPage> CcdListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new CcdListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), filterId_, userDefinedFilter(), userDefinedOrder(), parent));      
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> CcdListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {      
      std::auto_ptr<CardPage> cardPage(new CcdCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }

    void CcdListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void CcdListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    void CcdListPage::on_database_connectionStateChanged()
    {
      if (databaseModel()->isOpen())
      {
        model_->open();
      }
    }

    void CcdListPage::on_database_aboutToClose()
    {
      model_->close();
    }

    void CcdListPage::on_database_ccdInserted(const ma::chrono::model::Ccd& /*ccd*/)
    {
      model_->refresh();
    }

    void CcdListPage::on_database_ccdUpdated(const ma::chrono::model::Ccd& /*ccd*/)
    {
      model_->refresh();
    }

    void CcdListPage::on_database_ccdRemoved(const ma::chrono::model::Ccd& /*ccd*/)
    {
      model_->refresh();
    }

    void CcdListPage::on_database_contractUpdated(const ma::chrono::model::Contract& /*contract*/)
    {
      model_->refresh();
    }    

    void CcdListPage::on_database_counterpartUpdated(const ma::chrono::model::Counterpart& /*counterpart*/)
    {
      model_->refresh();
    }    

    void CcdListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      model_->refresh();
    }    

    void CcdListPage::on_database_deppointUpdated(const ma::chrono::model::Deppoint& /*deppoint*/)
    {
      model_->refresh();
    }    

    void CcdListPage::on_database_bankUpdated(const ma::chrono::model::Bank& /*bank*/)
    {
      model_->refresh();
    }    

    void CcdListPage::on_database_ttermUpdated(const ma::chrono::model::Tterm& /*tterm*/)
    {
      model_->refresh();
    }    
    
    void CcdListPage::on_database_currencyUpdated(const ma::chrono::model::Currency& /*currency*/)
    {
      model_->refresh();
    }       

  } // namespace chrono
} // namespace ma
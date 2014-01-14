/*
TRANSLATOR ma::chrono::BolListPage
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
#include <ma/chrono/boltablemodel.h>
#include <ma/chrono/bolcardpage.h>
#include <ma/chrono/model/bol.h>
#include <ma/chrono/model/ccd.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/counterpart.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/autoenterprise.h>
#include <ma/chrono/bollistpage.h>

namespace ma
{
  namespace chrono
  {    
    BolListPage::BolListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalInt& filterId,
      const UserDefinedFilter& userDefinedFilter,
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, bolEntity, 
          BolTableModel::tableDescription(), userDefinedFilter, userDefinedOrder, parent)
      , model_(new BolTableModel(databaseModel, this, filterId, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
      , filterId_(filterId)
    {    
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Bills of lading"));
      createAction()->setText(tr("&New bill of lading"));      
      setModel(model_);

      QObject::connect(databaseModel.get(), SIGNAL(connectionStateChanged()), SLOT(on_database_connectionStateChanged()));
      QObject::connect(databaseModel.get(), SIGNAL(aboutToClose()), SLOT(on_database_aboutToClose()));      
      QObject::connect(databaseModel.get(), SIGNAL(bolInserted(const ma::chrono::model::Bol&)), SLOT(on_database_bolInserted(const ma::chrono::model::Bol&)));
      QObject::connect(databaseModel.get(), SIGNAL(bolUpdated(const ma::chrono::model::Bol&)), SLOT(on_database_bolUpdated(const ma::chrono::model::Bol&)));
      QObject::connect(databaseModel.get(), SIGNAL(bolRemoved(const ma::chrono::model::Bol&)), SLOT(on_database_bolRemoved(const ma::chrono::model::Bol&)));
      QObject::connect(databaseModel.get(), SIGNAL(ccdUpdated(const ma::chrono::model::Ccd&)), SLOT(on_database_ccdUpdated(const ma::chrono::model::Ccd&)));
      QObject::connect(databaseModel.get(), SIGNAL(contractUpdated(const ma::chrono::model::Contract&)), SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));
      QObject::connect(databaseModel.get(), SIGNAL(counterpartUpdated(const ma::chrono::model::Counterpart&)), SLOT(on_database_counterpartUpdated(const ma::chrono::model::Counterpart&)));
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));                  
      QObject::connect(databaseModel.get(), SIGNAL(autoenterpriseUpdated(const ma::chrono::model::Autoenterprise&)), SLOT(on_database_autoenterpriseUpdated(const ma::chrono::model::Autoenterprise&)));

      on_database_connectionStateChanged();
    }

    BolListPage::~BolListPage()
    {
    }

    void BolListPage::refresh()
    {
      model_->refresh();
    }
        
    std::auto_ptr<ListPage> BolListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new BolListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), filterId_, userDefinedFilter(), userDefinedOrder(), parent));
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> BolListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {      
      std::auto_ptr<CardPage> cardPage(new BolCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    } 

    void BolListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void BolListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    void BolListPage::on_database_connectionStateChanged()
    {
      if (databaseModel()->isOpen())
      {
        model_->open();
      }
    }

    void BolListPage::on_database_aboutToClose()
    {
      model_->close();
    }

    void BolListPage::on_database_bolInserted(const ma::chrono::model::Bol& /*bol*/)
    {
      model_->refresh();
    }

    void BolListPage::on_database_bolUpdated(const ma::chrono::model::Bol& /*bol*/)
    {
      model_->refresh();
    }

    void BolListPage::on_database_bolRemoved(const ma::chrono::model::Bol& /*bol*/)
    {
      model_->refresh();
    }

    void BolListPage::on_database_ccdUpdated(const ma::chrono::model::Ccd& /*ccd*/)
    {
      model_->refresh();
    }

    void BolListPage::on_database_contractUpdated(const ma::chrono::model::Contract& /*contract*/)
    {
      model_->refresh();
    }    

    void BolListPage::on_database_counterpartUpdated(const ma::chrono::model::Counterpart& /*counterpart*/)
    {
      model_->refresh();
    }    

    void BolListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      model_->refresh();
    }        

    void BolListPage::on_database_autoenterpriseUpdated(const ma::chrono::model::Autoenterprise& /*autoenterprise*/)
    {
      model_->refresh();
    }

  } // namespace chrono
} // namespace ma
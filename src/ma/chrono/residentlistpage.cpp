/*
TRANSLATOR ma::chrono::ResidentListPage
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
#include <ma/chrono/residenttablemodel.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/country.h>
#include <ma/chrono/model/resident.h>
#include <ma/chrono/residentcardpage.h>
#include <ma/chrono/residentlistpage.h>

namespace ma
{
  namespace chrono
  {    
    ResidentListPage::ResidentListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction,       
      const UserDefinedFilter& userDefinedFilter,
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, residentEntity, 
          ResidentTableModel::tableDescription(), userDefinedFilter, userDefinedOrder, parent)
      , model_(new ResidentTableModel(databaseModel, this, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
    {       
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Residents"));
      createAction()->setText(tr("&New resident"));
      setModel(model_);

      QObject::connect(databaseModel.get(), SIGNAL(connectionStateChanged()), SLOT(on_database_connectionStateChanged()));
      QObject::connect(databaseModel.get(), SIGNAL(aboutToClose()), SLOT(on_database_aboutToClose()));      
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));      
      QObject::connect(databaseModel.get(), SIGNAL(countryUpdated(const ma::chrono::model::Country&)), SLOT(on_database_countryUpdated(const ma::chrono::model::Country&)));      
      QObject::connect(databaseModel.get(), SIGNAL(residentInserted(const ma::chrono::model::Resident&)), SLOT(on_database_residentInserted(const ma::chrono::model::Resident&)));
      QObject::connect(databaseModel.get(), SIGNAL(residentUpdated(const ma::chrono::model::Resident&)), SLOT(on_database_residentUpdated(const ma::chrono::model::Resident&)));
      QObject::connect(databaseModel.get(), SIGNAL(residentRemoved(const ma::chrono::model::Resident&)), SLOT(on_database_residentRemoved(const ma::chrono::model::Resident&)));

      on_database_connectionStateChanged();
    }

    ResidentListPage::~ResidentListPage()
    {
    }

    void ResidentListPage::refresh()
    {
      model_->refresh();      
    }
        
    std::auto_ptr<ListPage> ResidentListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new ResidentListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), userDefinedFilter(), userDefinedOrder(), parent));      
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> ResidentListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new ResidentCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }

    void ResidentListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void ResidentListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    OptionalQString ResidentListPage::relationalTextFromRow(int row) const
    {
      QVariant var = model_->data(model_->index(row, 2));
      if (!var.isNull())
      {          
        if (var.canConvert<QString>())
        {
          return var.toString();
        }        
      }              
      return OptionalQString();
    }

    void ResidentListPage::on_database_connectionStateChanged()
    {
      if (databaseModel()->isOpen())
      {
        model_->open();
      }
    }

    void ResidentListPage::on_database_aboutToClose()
    {
      model_->close();
    }    

    void ResidentListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      model_->refresh();
    }    

    void ResidentListPage::on_database_countryUpdated(const ma::chrono::model::Country& /*country*/)
    {
      model_->refresh();
    }    

    void ResidentListPage::on_database_residentInserted(const ma::chrono::model::Resident& /*resident*/)
    {
      model_->refresh();
    }

    void ResidentListPage::on_database_residentUpdated(const ma::chrono::model::Resident& /*resident*/)
    {
      model_->refresh();
    }

    void ResidentListPage::on_database_residentRemoved(const ma::chrono::model::Resident& /*resident*/)
    {
      model_->refresh();
    }

  } // namespace chrono
} // namespace ma
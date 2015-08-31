/*
TRANSLATOR ma::chrono::LcreditListPage
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
#include <ma/chrono/lcredittablemodel.h>
#include <ma/chrono/lcreditcardpage.h>
#include <ma/chrono/model/lcredit.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/counterpart.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/lcreditlistpage.h>

namespace ma
{
  namespace chrono
  {    
    LcreditListPage::LcreditListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalInt& filterId,
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, bolEntity, 
          LcreditTableModel::tableDescription(), userDefinedFilter, userDefinedOrder, parent)
      , model_(new LcreditTableModel(databaseModel, this, filterId, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
      , filterId_(filterId)
    {    
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Letters of credit"));
      createAction()->setText(tr("&New letter of credit"));      
      setModel(model_);

      QObject::connect(databaseModel.get(), SIGNAL(connectionStateChanged()), SLOT(on_database_connectionStateChanged()));
      QObject::connect(databaseModel.get(), SIGNAL(aboutToClose()), SLOT(on_database_aboutToClose()));      
      QObject::connect(databaseModel.get(), SIGNAL(lcreditInserted(const ma::chrono::model::Lcredit&)), SLOT(on_database_lcreditInserted(const ma::chrono::model::Lcredit&)));
      QObject::connect(databaseModel.get(), SIGNAL(lcreditUpdated(const ma::chrono::model::Lcredit&)), SLOT(on_database_lcreditUpdated(const ma::chrono::model::Lcredit&)));
      QObject::connect(databaseModel.get(), SIGNAL(lcreditRemoved(const ma::chrono::model::Lcredit&)), SLOT(on_database_lcreditRemoved(const ma::chrono::model::Lcredit&)));      
      QObject::connect(databaseModel.get(), SIGNAL(contractUpdated(const ma::chrono::model::Contract&)), SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));
      QObject::connect(databaseModel.get(), SIGNAL(counterpartUpdated(const ma::chrono::model::Counterpart&)), SLOT(on_database_counterpartUpdated(const ma::chrono::model::Counterpart&)));
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));                        

      on_database_connectionStateChanged();
    }

    LcreditListPage::~LcreditListPage()
    {
    }

    void LcreditListPage::refresh()
    {
      model_->refresh();
    }
        
    std::auto_ptr<ListPage> LcreditListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new LcreditListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), filterId_, userDefinedFilter(), userDefinedOrder(), parent));
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> LcreditListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {      
      std::auto_ptr<CardPage> cardPage(new LcreditCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    } 

    void LcreditListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void LcreditListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    void LcreditListPage::on_database_connectionStateChanged()
    {
      if (databaseModel()->isOpen())
      {
        model_->open();
      }
    }

    OptionalQString LcreditListPage::relationalTextFromRow(int row) const
    {
      if (QAbstractItemModel* model = this->model())
      {
        QVariant var = model->data(model->index(row, 3));        
        if (var.canConvert<QString>())
        {
          return var.toString();
        }
      }
      return OptionalQString();
    }

    void LcreditListPage::on_database_aboutToClose()
    {
      model_->close();
    }

    void LcreditListPage::on_database_lcreditInserted(const ma::chrono::model::Lcredit& /*lcredit*/)
    {
      model_->refresh();
    }

    void LcreditListPage::on_database_lcreditUpdated(const ma::chrono::model::Lcredit& /*lcredit*/)
    {
      model_->refresh();
    }

    void LcreditListPage::on_database_lcreditRemoved(const ma::chrono::model::Lcredit& /*lcredit*/)
    {
      model_->refresh();
    }    

    void LcreditListPage::on_database_contractUpdated(const ma::chrono::model::Contract& /*contract*/)
    {
      model_->refresh();
    }    

    void LcreditListPage::on_database_counterpartUpdated(const ma::chrono::model::Counterpart& /*counterpart*/)
    {
      model_->refresh();
    }    

    void LcreditListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      model_->refresh();
    }            

  } // namespace chrono
} // namespace ma
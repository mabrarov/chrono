/*
TRANSLATOR ma::chrono::TtermListPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <QAction>
#include <QTableView>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/ttermtablemodel.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/tterm.h>
#include <ma/chrono/ttermcardpage.h>
#include <ma/chrono/ttermlistpage.h>

namespace ma
{
  namespace chrono
  {    
    TtermListPage::TtermListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction,       
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, ttermEntity, 
          TtermTableModel::tableDescription(), userDefinedFilter, userDefinedOrder, parent)
      , model_(new TtermTableModel(databaseModel, this, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
    {       
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Transaction terms"));
      createAction()->setText(tr("&New transaction term"));    
      setModel(model_);      

      QObject::connect(databaseModel.get(), SIGNAL(connectionStateChanged()), SLOT(on_database_connectionStateChanged()));
      QObject::connect(databaseModel.get(), SIGNAL(aboutToClose()), SLOT(on_database_aboutToClose()));      
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));      
      QObject::connect(databaseModel.get(), SIGNAL(ttermInserted(const ma::chrono::model::Tterm&)), SLOT(on_database_ttermInserted(const ma::chrono::model::Tterm&)));
      QObject::connect(databaseModel.get(), SIGNAL(ttermUpdated(const ma::chrono::model::Tterm&)), SLOT(on_database_ttermUpdated(const ma::chrono::model::Tterm&)));
      QObject::connect(databaseModel.get(), SIGNAL(ttermRemoved(const ma::chrono::model::Tterm&)), SLOT(on_database_ttermRemoved(const ma::chrono::model::Tterm&)));

      on_database_connectionStateChanged();
    }

    TtermListPage::~TtermListPage()
    {
    }

    void TtermListPage::refresh()
    {
      model_->refresh();      
    }
        
    std::auto_ptr<ListPage> TtermListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new TtermListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), userDefinedFilter(), userDefinedOrder(), parent));      
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> TtermListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new TtermCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }    

    void TtermListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void TtermListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    void TtermListPage::on_database_connectionStateChanged()
    {
      if (databaseModel()->isOpen())
      {
        model_->open();
      }
    }

    void TtermListPage::on_database_aboutToClose()
    {
      model_->close();
    }    

    void TtermListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      model_->refresh();
    }    

    void TtermListPage::on_database_ttermInserted(const ma::chrono::model::Tterm& /*tterm*/)
    {
      model_->refresh();
    }

    void TtermListPage::on_database_ttermUpdated(const ma::chrono::model::Tterm& /*tterm*/)
    {
      model_->refresh();
    }

    void TtermListPage::on_database_ttermRemoved(const ma::chrono::model::Tterm& /*tterm*/)
    {
      model_->refresh();
    }

  } // namespace chrono
} //namespace ma
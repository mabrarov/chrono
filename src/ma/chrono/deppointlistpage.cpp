/*
TRANSLATOR ma::chrono::DeppointListPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <QAction>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/deppointtablemodel.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/deppoint.h>
#include <ma/chrono/deppointcardpage.h>
#include <ma/chrono/deppointlistpage.h>

namespace ma
{
  namespace chrono
  {    
    DeppointListPage::DeppointListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction,       
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, deppointEntity, 
          DeppointTableModel::tableDescription(), userDefinedFilter, userDefinedOrder, parent)
      , model_(new DeppointTableModel(databaseModel, this, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
    {      
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Departure points"));
      createAction()->setText(tr("&New departure point"));      
      setModel(model_);     

      QObject::connect(databaseModel.get(), SIGNAL(connectionStateChanged()), SLOT(on_database_connectionStateChanged()));
      QObject::connect(databaseModel.get(), SIGNAL(aboutToClose()), SLOT(on_database_aboutToClose()));      
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));      
      QObject::connect(databaseModel.get(), SIGNAL(deppointInserted(const ma::chrono::model::Deppoint&)), SLOT(on_database_deppointInserted(const ma::chrono::model::Deppoint&)));
      QObject::connect(databaseModel.get(), SIGNAL(deppointUpdated(const ma::chrono::model::Deppoint&)), SLOT(on_database_deppointUpdated(const ma::chrono::model::Deppoint&)));
      QObject::connect(databaseModel.get(), SIGNAL(deppointRemoved(const ma::chrono::model::Deppoint&)), SLOT(on_database_deppointRemoved(const ma::chrono::model::Deppoint&)));

      on_database_connectionStateChanged();
    }

    DeppointListPage::~DeppointListPage()
    {
    }

    void DeppointListPage::refresh()
    {
      model_->refresh();
    }
        
    std::auto_ptr<ListPage> DeppointListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new DeppointListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), userDefinedFilter(), userDefinedOrder(), parent));      
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> DeppointListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new DeppointCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    } 

    void DeppointListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void DeppointListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    OptionalQString DeppointListPage::relationalTextFromRow(int row) const
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

    void DeppointListPage::on_database_connectionStateChanged()
    {
      if (databaseModel()->isOpen())
      {
        model_->open();
      }
    }

    void DeppointListPage::on_database_aboutToClose()
    {
      model_->close();
    }    

    void DeppointListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      model_->refresh();
    }    

    void DeppointListPage::on_database_deppointInserted(const ma::chrono::model::Deppoint& /*deppoint*/)
    {
      model_->refresh();
    }

    void DeppointListPage::on_database_deppointUpdated(const ma::chrono::model::Deppoint& /*deppoint*/)
    {
      model_->refresh();
    }

    void DeppointListPage::on_database_deppointRemoved(const ma::chrono::model::Deppoint& /*deppoint*/)
    {
      model_->refresh();
    }

  } // namespace chrono
} //namespace ma
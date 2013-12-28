/*
TRANSLATOR ma::chrono::SbolListPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <QAction>
#include <QTableView>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/sboltablemodel.h>
#include <ma/chrono/sbolcardpage.h>
#include <ma/chrono/model/sbol.h>
#include <ma/chrono/model/ccd.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/counterpart.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/resident.h>
#include <ma/chrono/model/lcredit.h>
#include <ma/chrono/sbollistpage.h>

namespace ma
{
  namespace chrono
  {    
    SbolListPage::SbolListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalInt& filterId,
      const UserDefinedFilter& userDefinedFilter,
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, bolEntity, 
          SbolTableModel::tableDescription(), userDefinedFilter, userDefinedOrder, parent)
      , model_(new SbolTableModel(databaseModel, this, filterId, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
      , filterId_(filterId)
    {    
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Shipped bills of lading"));
      createAction()->setText(tr("&New shipped bill of lading"));      
      setModel(model_);

      QObject::connect(databaseModel.get(), SIGNAL(connectionStateChanged()), SLOT(on_database_connectionStateChanged()));
      QObject::connect(databaseModel.get(), SIGNAL(aboutToClose()), SLOT(on_database_aboutToClose()));      
      QObject::connect(databaseModel.get(), SIGNAL(sbolInserted(const ma::chrono::model::Sbol&)), SLOT(on_database_sbolInserted(const ma::chrono::model::Sbol&)));
      QObject::connect(databaseModel.get(), SIGNAL(sbolUpdated(const ma::chrono::model::Sbol&)), SLOT(on_database_sbolUpdated(const ma::chrono::model::Sbol&)));
      QObject::connect(databaseModel.get(), SIGNAL(sbolRemoved(const ma::chrono::model::Sbol&)), SLOT(on_database_sbolRemoved(const ma::chrono::model::Sbol&)));
      QObject::connect(databaseModel.get(), SIGNAL(ccdUpdated(const ma::chrono::model::Ccd&)), SLOT(on_database_ccdUpdated(const ma::chrono::model::Ccd&)));
      QObject::connect(databaseModel.get(), SIGNAL(contractUpdated(const ma::chrono::model::Contract&)), SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));
      QObject::connect(databaseModel.get(), SIGNAL(counterpartUpdated(const ma::chrono::model::Counterpart&)), SLOT(on_database_counterpartUpdated(const ma::chrono::model::Counterpart&)));
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));                  
      QObject::connect(databaseModel.get(), SIGNAL(residentUpdated(const ma::chrono::model::Resident&)), SLOT(on_database_residentUpdated(const ma::chrono::model::Resident&)));
      QObject::connect(databaseModel.get(), SIGNAL(lcreditUpdated(const ma::chrono::model::Lcredit&)), SLOT(on_database_lcreditUpdated(const ma::chrono::model::Lcredit&)));

      on_database_connectionStateChanged();
    }

    SbolListPage::~SbolListPage()
    {
    }

    void SbolListPage::refresh()
    {
      model_->refresh();
    }
        
    std::auto_ptr<ListPage> SbolListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new SbolListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), filterId_, userDefinedFilter(), userDefinedOrder(), parent));      
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> SbolListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {      
      std::auto_ptr<CardPage> cardPage(new SbolCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    } 

    void SbolListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void SbolListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    void SbolListPage::on_database_connectionStateChanged()
    {
      if (databaseModel()->isOpen())
      {
        model_->open();
      }
    }

    void SbolListPage::on_database_aboutToClose()
    {
      model_->close();
    }

    void SbolListPage::on_database_sbolInserted(const ma::chrono::model::Sbol& /*sbol*/)
    {
      model_->refresh();
    }

    void SbolListPage::on_database_sbolUpdated(const ma::chrono::model::Sbol& /*sbol*/)
    {
      model_->refresh();
    }

    void SbolListPage::on_database_sbolRemoved(const ma::chrono::model::Sbol& /*sbol*/)
    {
      model_->refresh();
    }

    void SbolListPage::on_database_ccdUpdated(const ma::chrono::model::Ccd& /*ccd*/)
    {
      model_->refresh();
    }

    void SbolListPage::on_database_contractUpdated(const ma::chrono::model::Contract& /*contract*/)
    {
      model_->refresh();
    }    

    void SbolListPage::on_database_counterpartUpdated(const ma::chrono::model::Counterpart& /*counterpart*/)
    {
      model_->refresh();
    }    

    void SbolListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      model_->refresh();
    }        

    void SbolListPage::on_database_residentUpdated(const ma::chrono::model::Resident& /*resident*/)
    {
      model_->refresh();
    }

    void SbolListPage::on_database_lcreditUpdated(const ma::chrono::model::Lcredit& /*lcredit*/)
    {
      model_->refresh();
    }

  } // namespace chrono
} //namespace ma
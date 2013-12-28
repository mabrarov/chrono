/*
TRANSLATOR ma::chrono::InsuranceListPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <QAction>
#include <QTableView>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/insurancetablemodel.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/insurance.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/currency.h>
#include <ma/chrono/insurancecardpage.h>
#include <ma/chrono/amountdelegate.h>
#include <ma/chrono/insurancelistpage.h>

namespace ma
{
  namespace chrono
  {    
    InsuranceListPage::InsuranceListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalInt& filterId,
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, insuranceEntity, 
          InsuranceTableModel::tableDescription(), userDefinedFilter, userDefinedOrder, parent)
      , model_(new InsuranceTableModel(databaseModel, this, filterId, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
      , filterId_(filterId)
    {    
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Insurances"));
      createAction()->setText(tr("&New insurance"));      
      setModel(model_);
      AmountDelegate* amountDelegate = new AmountDelegate(this);
      tableView()->setItemDelegateForColumn(4, amountDelegate);

      QObject::connect(databaseModel.get(), SIGNAL(connectionStateChanged()), SLOT(on_database_connectionStateChanged()));
      QObject::connect(databaseModel.get(), SIGNAL(aboutToClose()), SLOT(on_database_aboutToClose()));      
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));      
      QObject::connect(databaseModel.get(), SIGNAL(contractUpdated(const ma::chrono::model::Contract&)), SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));      
      QObject::connect(databaseModel.get(), SIGNAL(currencyUpdated(const ma::chrono::model::Currency&)), SLOT(on_database_currencyUpdated(const ma::chrono::model::Currency&)));      
      QObject::connect(databaseModel.get(), SIGNAL(insuranceInserted(const ma::chrono::model::Insurance&)), SLOT(on_database_insuranceInserted(const ma::chrono::model::Insurance&)));
      QObject::connect(databaseModel.get(), SIGNAL(insuranceUpdated(const ma::chrono::model::Insurance&)), SLOT(on_database_insuranceUpdated(const ma::chrono::model::Insurance&)));
      QObject::connect(databaseModel.get(), SIGNAL(insuranceRemoved(const ma::chrono::model::Insurance&)), SLOT(on_database_insuranceRemoved(const ma::chrono::model::Insurance&)));

      on_database_connectionStateChanged();
    }

    InsuranceListPage::~InsuranceListPage()
    {
    }

    void InsuranceListPage::refresh()
    {
      model_->refresh();      
    }
        
    std::auto_ptr<ListPage> InsuranceListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new InsuranceListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), filterId_, userDefinedFilter(), userDefinedOrder(), parent));
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> InsuranceListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {      
      std::auto_ptr<CardPage> cardPage(new InsuranceCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }

    void InsuranceListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void InsuranceListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    void InsuranceListPage::on_database_connectionStateChanged()
    {
      if (databaseModel()->isOpen())
      {
        model_->open();
      }
    }

    void InsuranceListPage::on_database_aboutToClose()
    {
      model_->close();
    }    

    void InsuranceListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      model_->refresh();
    }    

    void InsuranceListPage::on_database_contractUpdated(const ma::chrono::model::Contract& /*contract*/)
    {
      model_->refresh();
    }    

    void InsuranceListPage::on_database_currencyUpdated(const ma::chrono::model::Currency& /*currency*/)
    {
      model_->refresh();
    }    

    void InsuranceListPage::on_database_insuranceInserted(const ma::chrono::model::Insurance& /*insurance*/)
    {
      model_->refresh();
    }

    void InsuranceListPage::on_database_insuranceUpdated(const ma::chrono::model::Insurance& /*insurance*/)
    {
      model_->refresh();
    }

    void InsuranceListPage::on_database_insuranceRemoved(const ma::chrono::model::Insurance& /*insurance*/)
    {
      model_->refresh();
    }

  } // namespace chrono
} //namespace ma
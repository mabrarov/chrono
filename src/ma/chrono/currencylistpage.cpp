/*
TRANSLATOR ma::chrono::CurrencyListPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <QTableView>
#include <QAction>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/currencytablemodel.h>
#include <ma/chrono/currencycardpage.h>
#include <ma/chrono/currencynumericcodedelegate.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/currency.h>
#include <ma/chrono/currencylistpage.h>

namespace ma
{
  namespace chrono
  {    
    CurrencyListPage::CurrencyListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction,       
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, currencyEntity, 
          CurrencyTableModel::tableDescription(), userDefinedFilter, userDefinedOrder, parent)
      , model_(new CurrencyTableModel(databaseModel, this, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
    {       
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Currency"));      
      createAction()->setText(tr("&New currency"));
      setModel(model_);
      CurrencyNumericCodeDelegate* currencyNumericCodeDelegate = new CurrencyNumericCodeDelegate(this);
      tableView()->setItemDelegateForColumn(3, currencyNumericCodeDelegate);

      QObject::connect(databaseModel.get(), SIGNAL(connectionStateChanged()), SLOT(on_database_connectionStateChanged()));
      QObject::connect(databaseModel.get(), SIGNAL(aboutToClose()), SLOT(on_database_aboutToClose()));      
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));      
      QObject::connect(databaseModel.get(), SIGNAL(currencyInserted(const ma::chrono::model::Currency&)), SLOT(on_database_currencyInserted(const ma::chrono::model::Currency&)));
      QObject::connect(databaseModel.get(), SIGNAL(currencyUpdated(const ma::chrono::model::Currency&)), SLOT(on_database_currencyUpdated(const ma::chrono::model::Currency&)));
      QObject::connect(databaseModel.get(), SIGNAL(currencyRemoved(const ma::chrono::model::Currency&)), SLOT(on_database_currencyRemoved(const ma::chrono::model::Currency&)));

      on_database_connectionStateChanged();
    }

    CurrencyListPage::~CurrencyListPage()
    {
    }

    void CurrencyListPage::refresh()
    {
      model_->refresh();      
    }
        
    std::auto_ptr<ListPage> CurrencyListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new CurrencyListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), userDefinedFilter(), userDefinedOrder(), parent));      
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> CurrencyListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new CurrencyCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }

    void CurrencyListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void CurrencyListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    void CurrencyListPage::on_database_connectionStateChanged()
    {
      if (databaseModel()->isOpen())
      {
        model_->open();
      }
    }

    void CurrencyListPage::on_database_aboutToClose()
    {
      model_->close();
    }    

    void CurrencyListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      model_->refresh();
    }    

    void CurrencyListPage::on_database_currencyInserted(const ma::chrono::model::Currency& /*currency*/)
    {
      model_->refresh();
    }

    void CurrencyListPage::on_database_currencyUpdated(const ma::chrono::model::Currency& /*currency*/)
    {
      model_->refresh();
    }

    void CurrencyListPage::on_database_currencyRemoved(const ma::chrono::model::Currency& /*currency*/)
    {
      model_->refresh();
    }

  } // namespace chrono
} //namespace ma
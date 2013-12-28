/*
TRANSLATOR ma::chrono::CountryListPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <QAction>
#include <QTableView>
#include <ma/chrono/constants.h>
#include <ma/chrono/dbconstants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/countrytablemodel.h>
#include <ma/chrono/countrynumericcodedelegate.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/country.h>
#include <ma/chrono/countrycardpage.h>
#include <ma/chrono/countrylistpage.h>

namespace ma
{
  namespace chrono
  {    
    CountryListPage::CountryListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction,       
      const UserDefinedFilter& userDefinedFilter,
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, countryEntity, 
          CountryTableModel::tableDescription(), userDefinedFilter, userDefinedOrder, parent)
      , model_(new CountryTableModel(databaseModel, this, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
    {       
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Countries"));                 
      createAction()->setText(tr("&New country"));
      setModel(model_);
      CountryNumericCodeDelegate* countryNumericCodeDelegate = new CountryNumericCodeDelegate(this);
      tableView()->setItemDelegateForColumn(6, countryNumericCodeDelegate);

      QObject::connect(databaseModel.get(), SIGNAL(connectionStateChanged()), SLOT(on_database_connectionStateChanged()));
      QObject::connect(databaseModel.get(), SIGNAL(aboutToClose()), SLOT(on_database_aboutToClose()));      
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));      
      QObject::connect(databaseModel.get(), SIGNAL(countryInserted(const ma::chrono::model::Country&)), SLOT(on_database_countryInserted(const ma::chrono::model::Country&)));
      QObject::connect(databaseModel.get(), SIGNAL(countryUpdated(const ma::chrono::model::Country&)), SLOT(on_database_countryUpdated(const ma::chrono::model::Country&)));
      QObject::connect(databaseModel.get(), SIGNAL(countryRemoved(const ma::chrono::model::Country&)), SLOT(on_database_countryRemoved(const ma::chrono::model::Country&)));

      on_database_connectionStateChanged();
    }

    CountryListPage::~CountryListPage()
    {
    }

    void CountryListPage::refresh()
    {
      model_->refresh();
    }
        
    std::auto_ptr<ListPage> CountryListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new CountryListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), userDefinedFilter(), userDefinedOrder(), parent));      
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }
     
    std::auto_ptr<CardPage> CountryListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {      
      std::auto_ptr<CardPage> cardPage(new CountryCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));      
      return cardPage;
    }

    void CountryListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void CountryListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    void CountryListPage::on_database_connectionStateChanged()
    {
      if (databaseModel()->isOpen())
      {
        model_->open();
      }
    }

    void CountryListPage::on_database_aboutToClose()
    {
      model_->close();
    }    

    void CountryListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      model_->refresh();
    }    

    void CountryListPage::on_database_countryInserted(const ma::chrono::model::Country& /*country*/)
    {
      model_->refresh();
    }

    void CountryListPage::on_database_countryUpdated(const ma::chrono::model::Country& /*country*/)
    {
      model_->refresh();
    }

    void CountryListPage::on_database_countryRemoved(const ma::chrono::model::Country& /*country*/)
    {
      model_->refresh();
    }

  } // namespace chrono
} //namespace ma
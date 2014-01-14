/*
TRANSLATOR ma::chrono::IctListPage
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
#include <ma/chrono/icttablemodel.h>
#include <ma/chrono/ictcardpage.h>
#include <ma/chrono/model/ict.h>
//#include <ma/chrono/model/contract.h>
//#include <ma/chrono/model/counterpart.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/ictlistpage.h>

namespace ma
{
  namespace chrono
  {    
    IctListPage::IctListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalInt& filterId,
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder, 
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, ictEntity, 
          IctTableModel::tableDescription(), userDefinedFilter, userDefinedOrder, parent)
      , model_(new IctTableModel(databaseModel, this, filterId, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
      , filterId_(filterId)
    {    
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Inquiries on currency transactions"));
      createAction()->setText(tr("&New inquiry on currency transactions"));      
      setModel(model_);

      QObject::connect(databaseModel.get(), SIGNAL(connectionStateChanged()), SLOT(on_database_connectionStateChanged()));
      QObject::connect(databaseModel.get(), SIGNAL(aboutToClose()), SLOT(on_database_aboutToClose()));      
      QObject::connect(databaseModel.get(), SIGNAL(ictInserted(const ma::chrono::model::Ict&)), SLOT(on_database_ictInserted(const ma::chrono::model::Ict&)));
      QObject::connect(databaseModel.get(), SIGNAL(ictUpdated(const ma::chrono::model::Ict&)), SLOT(on_database_ictUpdated(const ma::chrono::model::Ict&)));
      QObject::connect(databaseModel.get(), SIGNAL(ictRemoved(const ma::chrono::model::Ict&)), SLOT(on_database_ictRemoved(const ma::chrono::model::Ict&)));      
      //QObject::connect(databaseModel.get(), SIGNAL(contractUpdated(const ma::chrono::model::Contract&)), SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));
      //QObject::connect(databaseModel.get(), SIGNAL(counterpartUpdated(const ma::chrono::model::Counterpart&)), SLOT(on_database_counterpartUpdated(const ma::chrono::model::Counterpart&)));
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));                        

      on_database_connectionStateChanged();
    }

    IctListPage::~IctListPage()
    {
    }

    void IctListPage::refresh()
    {
      model_->refresh();
    }
        
    std::auto_ptr<ListPage> IctListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new IctListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), filterId_, userDefinedFilter(), userDefinedOrder(), parent));
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> IctListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {      
      std::auto_ptr<CardPage> cardPage(new IctCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    } 

    void IctListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void IctListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    void IctListPage::on_database_connectionStateChanged()
    {
      if (databaseModel()->isOpen())
      {
        model_->open();
      }
    }

    OptionalQString IctListPage::relationalTextFromRow(int row) const
    {
      if (QAbstractItemModel* model = this->model())
      {
        QVariant bankShortNameVar = model->data(model->index(row, 1));
        if (bankShortNameVar.canConvert<QString>())
        {
          QVariant dateVar = model->data(model->index(row, 2));
          if (dateVar.canConvert<QDate>())
          {
            QString bankShortName = bankShortNameVar.toString();
            QDate date = dateVar.toDate();
            static QString format(tr("By %1 for %2"));
            return format.arg(date.toString(Qt::DefaultLocaleShortDate)).arg(bankShortName);
          }
        }
      }
      return OptionalQString();
    }

    void IctListPage::on_database_aboutToClose()
    {
      model_->close();
    }

    void IctListPage::on_database_ictInserted(const ma::chrono::model::Ict& /*ict*/)
    {
      model_->refresh();
    }

    void IctListPage::on_database_ictUpdated(const ma::chrono::model::Ict& /*ict*/)
    {
      model_->refresh();
    }

    void IctListPage::on_database_ictRemoved(const ma::chrono::model::Ict& /*ict*/)
    {
      model_->refresh();
    }    

    //void IctListPage::on_database_contractUpdated(const ma::chrono::model::Contract& /*contract*/)
    //{
    //  model_->refresh();
    //}    

    //void IctListPage::on_database_counterpartUpdated(const ma::chrono::model::Counterpart& /*counterpart*/)
    //{
    //  model_->refresh();
    //}    

    void IctListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      model_->refresh();
    }            

  } // namespace chrono
} // namespace ma
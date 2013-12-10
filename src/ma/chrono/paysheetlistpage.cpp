/*
TRANSLATOR ma::chrono::PaysheetListPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <QAction>
#include <QTableView>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/paysheettablemodel.h>
#include <ma/chrono/paysheetcardpage.h>
#include <ma/chrono/model/paysheet.h>
#include <ma/chrono/amountdelegate.h>
#include <ma/chrono/model/paysheet.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/ccd.h>
#include <ma/chrono/model/currency.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/paysheetlistpage.h>

namespace ma
{
  namespace chrono
  {    
    PaysheetListPage::PaysheetListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const OptionalInt& filterId,
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, paysheetEntity, 
          PaysheetTableModel::tableDescription(), userDefinedFilter, userDefinedOrder, parent)
      , model_(new PaysheetTableModel(databaseModel, this, filterId, 
          userDefinedFilter.getUserDefinedFilterExpression(),
          userDefinedOrder.getUserDefinedOrderExpression()))
      , filterId_(filterId)
    {    
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Pay sheets"));
      createAction()->setText(tr("&New pay sheet"));      
      setModel(model_);
      AmountDelegate* amountDelegate = new AmountDelegate(this);
      tableView()->setItemDelegateForColumn(6, amountDelegate);

      QObject::connect(databaseModel.get(), SIGNAL(connectionStateChanged()), SLOT(on_database_connectionStateChanged()));
      QObject::connect(databaseModel.get(), SIGNAL(aboutToClose()), SLOT(on_database_aboutToClose()));      
      QObject::connect(databaseModel.get(), SIGNAL(paysheetInserted(const ma::chrono::model::Paysheet&)), SLOT(on_database_paysheetInserted(const ma::chrono::model::Paysheet&)));
      QObject::connect(databaseModel.get(), SIGNAL(paysheetUpdated(const ma::chrono::model::Paysheet&)), SLOT(on_database_paysheetUpdated(const ma::chrono::model::Paysheet&)));      
      QObject::connect(databaseModel.get(), SIGNAL(paysheetRemoved(const ma::chrono::model::Paysheet&)), SLOT(on_database_paysheetRemoved(const ma::chrono::model::Paysheet&)));
      QObject::connect(databaseModel.get(), SIGNAL(contractUpdated(const ma::chrono::model::Contract&)), SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));
      QObject::connect(databaseModel.get(), SIGNAL(ccdUpdated(const ma::chrono::model::Ccd&)), SLOT(on_database_ccdUpdated(const ma::chrono::model::Ccd&)));
      QObject::connect(databaseModel.get(), SIGNAL(currencyUpdated(const ma::chrono::model::Currency&)), SLOT(on_database_currencyUpdated(const ma::chrono::model::Currency&)));
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));                        

      on_database_connectionStateChanged();
    }

    PaysheetListPage::~PaysheetListPage()
    {
    }

    void PaysheetListPage::refresh()
    {
      model_->refresh();
    }
        
    std::auto_ptr<ListPage> PaysheetListPage::clone(QWidget* parent) const
    {
      std::auto_ptr<ListPage> clonedPage(new PaysheetListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), filterId_, userDefinedFilter(), userDefinedOrder(), parent));
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

    std::auto_ptr<CardPage> PaysheetListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {      
      std::auto_ptr<CardPage> cardPage(new PaysheetCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    } 

    void PaysheetListPage::applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      model_->setFilterExpression(userDefinedFilter.getUserDefinedFilterExpression());
    }

    void PaysheetListPage::applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      model_->setOrderExpression(userDefinedOrder.getUserDefinedOrderExpression());
    }

    void PaysheetListPage::on_database_connectionStateChanged()
    {
      if (databaseModel()->isOpen())
      {
        model_->open();
      }
    }

    OptionalQString PaysheetListPage::relationalTextFromRow(int row) const
    {
      if (QAbstractItemModel* model = this->model())
      {
        QVariant docNumberVar = model->data(model->index(row, 4));
        if (docNumberVar.canConvert<QString>())
        {
          QVariant dateVar = model->data(model->index(row, 5));
          if (dateVar.canConvert<QDate>())
          {
            QString docNumber = docNumberVar.toString();
            QDate date = dateVar.toDate();
            static QString format(tr("%1 by %2"));
            return format.arg(docNumber).arg(date.toString(Qt::DefaultLocaleShortDate));          
          }
        }
      }
      return OptionalQString();
    }

    void PaysheetListPage::on_database_aboutToClose()
    {
      model_->close();
    }

    void PaysheetListPage::on_database_paysheetInserted(const ma::chrono::model::Paysheet& /*paysheet*/)
    {
      model_->refresh();
    }

    void PaysheetListPage::on_database_paysheetUpdated(const ma::chrono::model::Paysheet& /*paysheet*/)
    {
      model_->refresh();
    }

    void PaysheetListPage::on_database_paysheetRemoved(const ma::chrono::model::Paysheet& /*paysheet*/)
    {
      model_->refresh();
    }    

    void PaysheetListPage::on_database_contractUpdated(const ma::chrono::model::Contract& /*contract*/)
    {
      model_->refresh();
    }    

    void PaysheetListPage::on_database_ccdUpdated(const ma::chrono::model::Ccd& /*ccd*/)
    {
      model_->refresh();
    }    

    void PaysheetListPage::on_database_currencyUpdated(const ma::chrono::model::Currency& /*Currency*/)
    {
      model_->refresh();
    }    

    void PaysheetListPage::on_database_userUpdated(const ma::chrono::model::User& /*user*/)
    {
      model_->refresh();
    }            

  } // namespace chrono
} //namespace ma
//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_CARDPAGE_H
#define MA_CHRONO_CARDPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <memory>
#include <boost/function.hpp>
#include <qglobal.h>
#include <ma/chrono/types.h>
#include <ma/chrono/dbconstants.h>
#include <ma/chrono/datapage.h>
#include <ma/chrono/exportdatalist.h>
#include <ma/chrono/cardpage_fwd.h>

QT_BEGIN_NAMESPACE
class QMenu; 
class QDateEdit;
QT_END_NAMESPACE

namespace ma {
namespace chrono {

class CardPage : public DataPage
{
  Q_OBJECT

public:
  enum Mode
  {
    viewMode,
    editMode,
    createMode
  }; // enum Mode

  CardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Entity entity, 
      Mode mode, const OptionalQInt64& entityId, QWidget* parent = 0);
  ~CardPage();  

  Mode mode() const
  {
    return mode_;
  }

  Entity entity() const
  {
    return entity_;
  }

  OptionalQInt64 entityId() const
  {
    return entityId_;
  }

  QActionList primaryActions() const
  {
    return primaryActions_;
  }

  QAction* customizeAction() const
  {
    return customizeAction_;
  }

  QActionList contextActions() const
  {
    return contextActions_;
  }

  QAction* saveAction() const
  {
    return saveAction_;
  }

  QAction* cancelAction() const
  {
    return cancelAction_;
  }

  QAction* editAction() const
  {
    return editAction_;
  }

  QAction* createAction() const
  {
    return createAction_;
  }

  QAction* deleteAction() const
  {
    return deleteAction_;
  }

  QAction* refreshAction() const
  {
    return refreshAction_;
  }

  QAction* sendToExcelAction() const
  {
    return sendToExcelAction_;
  }

  QAction* printPageAction() const
  {
    return printPageAction_;
  }

  void addPrimaryActions(const QActionList& actions);
  void addSpecialActions(const QActionList& actions);

  virtual std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;    
  virtual void save();
  virtual void cancel();
  virtual void edit();      
  virtual void remove();      

signals:
  void modeChanged();      

protected:
  void showUserCardTaskWindow(qint64 userId); 
  void showCcdCardTaskWindow(qint64 ccdId);         
  void showCounterpartCardTaskWindow(qint64 counterpartId);         
  void showAutoenterpriseCardTaskWindow(qint64 autoenterpriseId);      
  void showCurrencyCardTaskWindow(qint64 currencyId);   
  void showContractCardTaskWindow(qint64 contractId);   
  void showDeppointCardTaskWindow(qint64 deppointId);                     
  void showBankCardTaskWindow(qint64 bankId);         
  void showTtermCardTaskWindow(qint64 ttermId);
  void showResidentCardTaskWindow(qint64 residentId); 
  void showCountryCardTaskWindow(qint64 countryId); 
  void showPaysheetCardTaskWindow(qint64 paysheetId);         
  void showLcreditCardTaskWindow(qint64 lcreditId);
  void setEntityId(const OptionalQInt64& entityId);      
  void setExportDataList(const ExportDataList& exportDataList);
            
private:
  typedef boost::function<
      std::auto_ptr<CardPage> (const ResourceManagerPtr&, 
          const TaskWindowManagerPtr&, const DatabaseModelPtr&,
          const QActionSharedPtr&, Mode, qint64)> CardPageFactory;

  Q_DISABLE_COPY(CardPage)

  void showCardTaskWindow(Entity entity, qint64 entityId, 
      CardPageFactory cardPageFactory);

  void setMode(Mode mode);
  void updateActions();
           
  Entity         entity_;
  OptionalQInt64 entityId_;
  Mode           mode_;
  QMenu*         actionsMenu_;
  QAction*       saveAction_;
  QAction*       cancelAction_;
  QAction*       editAction_;
  QAction*       createAction_;
  QAction*       deleteAction_;
  QAction*       processActionsSeparator_;      
  QAction*       specialActionsSeparator_;      
  QAction*       refreshAction_;      
  QAction*       sendToExcelAction_;      
  QAction*       commonActionsSeparator_;
  QAction*       printPageAction_;
  QActionList    primaryActions_;
  QAction*       customizeAction_;
  QActionList    contextActions_;
  QActionList    dataAwareActions_;
  ExportDataList exportDataList_;

private slots:      
  void on_saveAction_triggered();
  void on_cancelAction_triggered();
  void on_editAction_triggered();
  void on_createAction_triggered();
  void on_deleteAction_triggered();
  void on_refreshAction_triggered();            
  virtual void on_sendToExcelAction_triggered();
  void on_printPageAction_triggered();      
  void on_databaseModel_connectionStateChanged();
}; // class CardPage    

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_CARDPAGE_H

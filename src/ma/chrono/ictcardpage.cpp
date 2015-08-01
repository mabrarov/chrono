/*
TRANSLATOR ma::chrono::IctCardPage
*/

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/bind.hpp>
#include <QCoreApplication>
#include <QFile>
#include <QAction>
#include <QMessageBox>
#include <QCalendarWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMenu>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <ma/chrono/constants.h>
#include <ma/chrono/dbconstants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/listselectdialog.h>
#include <ma/chrono/banklistpage.h>
#include <ma/chrono/paysheetlistpage.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/widgetutility.h>
#include <ma/chrono/sqlutility.h>
#include <ma/chrono/model/bank.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/currencyamount.h>
#include <ma/chrono/model/paysheet.h>
#include <ma/chrono/model/currency.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/ccd.h>
#include <ma/chrono/localeutility.h>
#include <ma/chrono/excelexporter.h>
#include <ma/chrono/ictcardpage.h>

namespace ma {

namespace chrono {

namespace {

  const int maxPayheets = 30;

}

IctCardPage::IctCardPage(const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerWeakPtr& taskWindowManager,
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, Mode mode, 
    const OptionalQInt64& entityId, QWidget* parent)
  : CardPage(resourceManager, taskWindowManager, databaseModel, 
        helpAction, ictEntity, mode, entityId, parent)
  , dataAwareWidgets_()
  , contentsChanged_(false)     
{
  ui_.setupUi(this);          
  ui_.tabWidget->setCurrentIndex(0);
  setWindowIcon(resourceManager->getIcon(chronoIconName));
  LocaleUtility::setupLocaleAwareWidget(ui_.ictDateEdit);
  LocaleUtility::setupLocaleAwareWidget(ui_.recvDateEdit);            
  ui_.moveUpBtn->setIcon(resourceManager->getIcon(moveUpIconName));
  ui_.moveDownBtn->setIcon(resourceManager->getIcon(moveDownIconName));            
  createAction()->setText(tr("&New inquiry on currency transactions"));
  dataAwareWidgets_ << ui_.ictDateEdit 
                    << ui_.recvDateEdit 
                    << ui_.stateCheck 
                    <<  ui_.remarksEdit;
  requiredMarkers_  << ui_.asterikLabel 
                    << ui_.asterikLabel_2 
                    << ui_.asterikLabel_3;
  connectDataAwareWidgets();
  if (entityId)
  {
    if (populateFromDatabase())
    {
      populateWidgets();
    }
    else
    {
      setEntityId(OptionalQInt64());
    }
  }      

  QHeaderView* paysheetTableHeader = ui_.paysheetTable->horizontalHeader();

  // Qt 5 migration
#if QT_VERSION >= 0x050000      
  paysheetTableHeader->setSectionsMovable(true);
#else
  paysheetTableHeader->setMovable(true);
#endif

  paysheetTableHeader->setContextMenuPolicy(Qt::CustomContextMenu);
  paysheetTableHeader->setHighlightSections(false);      
  paysheetTableHeader->setStretchLastSection(true);
  QObject::connect(paysheetTableHeader, 
      SIGNAL(customContextMenuRequested(const QPoint&)), 
      SLOT(on_paysheetTable_headerContextMenu_requested(const QPoint&)));
      
  QObject::connect(databaseModel.get(), 
      SIGNAL(userUpdated(const ma::chrono::model::User&)),
      SLOT(on_database_userUpdated(const ma::chrono::model::User&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(bankUpdated(const ma::chrono::model::Bank&)), 
      SLOT(on_database_bankUpdated(const ma::chrono::model::Bank&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(bankRemoved(const ma::chrono::model::Bank&)), 
      SLOT(on_database_bankRemoved(const ma::chrono::model::Bank&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(paysheetUpdated(const ma::chrono::model::Paysheet&)), 
      SLOT(on_database_paysheetUpdated(const ma::chrono::model::Paysheet&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(paysheetRemoved(const ma::chrono::model::Paysheet&)), 
      SLOT(on_database_paysheetRemoved(const ma::chrono::model::Paysheet&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(currencyUpdated(const ma::chrono::model::Currency&)), 
      SLOT(on_database_currencyUpdated(const ma::chrono::model::Currency&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(contractUpdated(const ma::chrono::model::Contract&)),
      SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(ccdUpdated(const ma::chrono::model::Ccd&)), 
      SLOT(on_database_ccdUpdated(const ma::chrono::model::Ccd&)));
  updateWidgets();         
}

IctCardPage::~IctCardPage()
{
} 

void IctCardPage::refresh()
{
  switch (mode())
  {
  case viewMode:
  case editMode:
    if (populateFromDatabase())
    {
      populateWidgets();
    }        
    break;      

  case createMode:        
    break;      
  }
  updateWidgets();
}

void IctCardPage::save()
{
  switch (mode())
  {
  case editMode:
    if (populateFromWidgets())
    {
      if (updateDatabase())
      {
        if (populateFromDatabase())
        {
          populateWidgets();
          CardPage::save();
        }
      }          
    }        
    break;

  case createMode:
    if (populateFromWidgets())
    {
      if (insertIntoDatabase())
      {            
        if (populateFromDatabase())
        {
          populateWidgets();
          CardPage::save();
        }
      }          
    }        
    break;

  default:
    CardPage::save();
    break;
  } 
  updateWidgets();
}

void IctCardPage::cancel()
{
  if (editMode == mode())
  {      
    populateWidgets();
  }      
  CardPage::cancel();
  updateWidgets();
}

void IctCardPage::edit()
{      
  CardPage::edit();
  updateWidgets();
}

void IctCardPage::remove()
{
  if (createMode != mode())
  {
    if (removeFromDatabase())
    {
      CardPage::remove();
    }
  }            
}

std::auto_ptr<CardPage> IctCardPage::createCardPage(CardPage::Mode mode,
    const OptionalQInt64& entityId) const
{
  std::auto_ptr<CardPage> cardPage(new IctCardPage(resourceManager(), 
      taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
  return cardPage;
}

void IctCardPage::connectDataAwareWidgets()
{
  typedef QWidgetList::const_iterator const_iterator;
      
  for (const_iterator it = dataAwareWidgets_.begin(), 
      end = dataAwareWidgets_.end(); it != end; ++it)
  {
    QWidget* widget = *it;        
    if (QLineEdit* edit = qobject_cast<QLineEdit*>(widget))
    {
      QObject::connect(edit, 
          SIGNAL(textEdited(const QString&)), 
          SLOT(on_lineEdit_textEdited(const QString&)));
    }
    else if (QTextEdit* edit = qobject_cast<QTextEdit*>(widget))
    {
      QObject::connect(edit, 
          SIGNAL(textChanged()), SLOT(on_textEdit_textChanged()));
    }        
    else if (QDateEdit* edit = qobject_cast<QDateEdit*>(widget))
    {
      QObject::connect(edit, 
          SIGNAL(dateChanged(const QDate&)), 
          SLOT(on_dateEdit_dateChanged(const QDate&)));
    }
  }
}

void IctCardPage::updateWidgets()
{      
      
  typedef QWidgetList::const_iterator const_iterator;

  Mode currentMode = mode();
  bool readOnly = viewMode == currentMode;
  for (const_iterator it = dataAwareWidgets_.begin(), 
      end = dataAwareWidgets_.end(); it != end; ++it)
  {
    QWidget* widget = *it;        
    if (QLineEdit* edit = qobject_cast<QLineEdit*>(widget))
    {
      edit->setReadOnly(readOnly);          
    }
    else if (QAbstractSpinBox* edit = qobject_cast<QAbstractSpinBox*>(widget))
    {
      edit->setReadOnly(readOnly);          
    }
    else if (QTextEdit* edit = qobject_cast<QTextEdit*>(widget))
    {
      edit->setReadOnly(readOnly);          
    }  
    else 
    {
      widget->setEnabled(!readOnly);
    } 
  }
  bool peristance = createMode != currentMode && entityId();      
  int generalTabIndex = ui_.tabWidget->indexOf(ui_.generalTab);      
  ui_.tabWidget->setTabEnabled(generalTabIndex, peristance);            
  switch (currentMode)
  {
  case viewMode:
    if (entityId())
    {          
      updateWindowTitle(tr("Inquiries on currency transactions - By %1 for %2")
          .arg(entityData_.ictDate.toString(Qt::DefaultLocaleShortDate))
          .arg(bankShortName_));
    }
    ui_.paysheetTableModifyPanel->setVisible(false);        
    showRequiredMarkers(false);
    break;

  case editMode:
    if (entityId())
    {
      if (contentsChanged_)
      {
        updateWindowTitle(tr("Inquiries on currency transactions - By %1 for %2* - edit")
            .arg(entityData_.ictDate.toString(Qt::DefaultLocaleShortDate))
            .arg(bankShortName_));            
      }
      else
      {
        updateWindowTitle(tr("Inquiries on currency transactions - By %1 for %2 - edit")
            .arg(entityData_.ictDate.toString(Qt::DefaultLocaleShortDate))
            .arg(bankShortName_));            
      }        
    }
    ui_.paysheetTableModifyPanel->setVisible(true);        
    showRequiredMarkers(true);
    break;

  case createMode:        
    updateWindowTitle(tr("Inquiries on currency transactions - New inquiry on currency transactions*"));
    ui_.paysheetTableModifyPanel->setVisible(true);
    showRequiredMarkers(true);
    break;
  }
  bool confirmed = Qt::Checked == ui_.stateCheck->checkState();
  ui_.recvDateLabel->setVisible(confirmed);
  ui_.recvDateEdit->setVisible(confirmed);      
  updatePaysheetTableWidgets();
}

bool IctCardPage::populateFromWidgets()
{
  static const char* bankTitle = QT_TR_NOOP("Bank");            
  static const char* paysheetTableTitle = QT_TR_NOOP("Pay sheets");
  static const char* remarkTitle = QT_TR_NOOP("Remark");      
      
  static const char* messageTitle = QT_TR_NOOP("Invalid data input");
  static const char* notEmpty = QT_TR_NOOP("Field \"%1\" can\'t be empty.");
  static const char* listNotEmpty = QT_TR_NOOP("List \"%1\" can\'t be empty.");
  static const char* tooLongText = QT_TR_NOOP("Field \"%1\" can\'t exceed %2 characters.");      
            
  int contentsTabIndex = ui_.tabWidget->indexOf(ui_.contentsTab);
  // Get bank
  if (!selectedBankId_)
  {        
    QMessageBox::warning(this, tr(messageTitle), 
        tr(notEmpty).arg(tr(bankTitle)));
    ui_.tabWidget->setCurrentIndex(contentsTabIndex);
    ui_.bankBtn->setFocus(Qt::TabFocusReason);
    return false;
  }  
  if (nonPersistPaysheetTableData_.isEmpty())
  {
    QMessageBox::warning(this, tr(messageTitle), 
        tr(listNotEmpty).arg(tr(paysheetTableTitle)));
    ui_.tabWidget->setCurrentIndex(contentsTabIndex);
    ui_.addPaysheetBtn->setFocus(Qt::TabFocusReason);
    return false;
  }      
  // Get remark      
  QString remark = ui_.remarksEdit->toPlainText().trimmed();
  if (remark.length() > model::Ict::remarkMaxLen)
  {
    int remarksTabIndex = ui_.tabWidget->indexOf(ui_.remarksTab);
    QMessageBox::warning(this, tr(messageTitle), 
        tr(tooLongText).arg(tr(remarkTitle)).arg(model::Ict::remarkMaxLen));
    ui_.tabWidget->setCurrentIndex(remarksTabIndex);
    ui_.remarksEdit->setFocus(Qt::TabFocusReason);
    ui_.remarksEdit->selectAll();
    return false;
  }
  entityData_.ictDate  = ui_.ictDateEdit->date();
  entityData_.bankId   = *selectedBankId_;
  entityData_.state    = Qt::Checked == ui_.stateCheck->checkState() ? 1 : 0;
  entityData_.recvDate = Qt::Checked == 
      ui_.stateCheck->checkState() ? ui_.recvDateEdit->date() : OptionalQDate();
  entityData_.remark = remark.isEmpty() ? OptionalQString() : remark;
  paysheetTableData_ = nonPersistPaysheetTableData_;
  return true;
}

bool IctCardPage::populateFromDatabase()
{      
  static const char* querySql = "select " \
      "t.\"BANK_ID\", b.\"SHORT_NAME\", " \
      "t.\"ICT_DATE\", t.\"STATE\", t.\"RECV_DATE\", " \
      "t.\"REMARK\", " \
      "cu.\"LOGIN\", uu.\"LOGIN\", " \
      "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", " \
      "t.\"CREATE_USER_ID\", t.\"UPDATE_USER_ID\", b.\"BANK_CODE\" " \
      "from \"ICT\" t " \
      "join \"BANK\" b on t.\"BANK_ID\" = b.\"BANK_ID\" " \
      "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
      "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
      "where t.\"ICT_ID\" = ?";

  static const char* paysheetQuerySql = "select " \
      "rel.\"PAY_SHEET_ID\", " \
      "t.\"CONTRACT_ID\", cr.\"NUMBER\", " \
      "t.\"CCD_ID\", c.\"NUMBER\", " \
      "t.\"DOC_NUMBER\", t.\"PAY_DATE\", t.\"AMOUNT\", " \
      "cur.\"ALPHABETIC_CODE\", t.\"ACCOUNT_NUMBER\", t.\"REMARK\", " \
      "t.\"CREATE_USER_ID\", t.\"UPDATE_USER_ID\", " \
      "cu.\"LOGIN\", uu.\"LOGIN\", " \
      "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", t.\"CURRENCY_ID\", " \
      "cr.\"PASSPORT_NUMBER\" " \
      "from \"PAYSHEET_ICT_RELATION\" rel " \
      "join \"PAY_SHEET\" t on t.\"PAY_SHEET_ID\" = rel.\"PAY_SHEET_ID\" " \
      "join \"CONTRACT\" cr on t.\"CONTRACT_ID\" = cr.\"CONTRACT_ID\" " \
      "left join \"CCD\" c on t.\"CCD_ID\" = c.\"CCD_ID\" " \
      "join \"CURRENCY\" cur on t.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
      "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
      "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
      "where rel.\"ICT_ID\" = ? order by rel.\"TAB_ORDER\"";

  if (entityId())
  {      
    QSqlDatabase database(databaseModel()->database());        
    if (database.transaction())
    {        
      QSqlQuery query(database);
      QSqlQuery paysheetQuery(database);
      if (query.prepare(querySql) && paysheetQuery.prepare(paysheetQuerySql))
      {
        query.bindValue(0, entityId().get());
        paysheetQuery.bindValue(0, entityId().get());
        if (query.exec() && paysheetQuery.exec())
        {
          if (query.next())
          { 
            entityData_.ictId    = entityId().get();
            entityData_.bankId   = query.value(0).toLongLong();
            bankShortName_       = query.value(1).toString();
            entityData_.ictDate  = 
                databaseModel()->convertFromServer(query.value(2).toDate());
            entityData_.state    = query.value(3).toInt();
            entityData_.recvDate = query.isNull(4) ? OptionalQDate() 
                : databaseModel()->convertFromServer(query.value(4).toDate());
            entityData_.remark = query.isNull(5) ? OptionalQString() 
                : query.value(5).toString();              
            createUserName_      = query.value(6).toString(); 
            updateUserName_      = query.value(7).toString(); 
            entityData_.createTime   = databaseModel()->convertFromServer(
                query.value(8).toDateTime()); 
            entityData_.updateTime   = databaseModel()->convertFromServer(
                query.value(9).toDateTime());
            entityData_.createUserId = query.value(10).toLongLong();
            entityData_.updateUserId = query.value(11).toLongLong();
            bankCode_ = query.value(12).toInt();
            paysheetTableData_ = readPaysheetData(paysheetQuery);
            if (!database.commit())
            {
              database.rollback();
            }
            return true;
          }
        }
      }
      database.rollback();
    } // if (database.transaction())
  } // if (entityId())
  return false;
}

void IctCardPage::populateWidgets()
{
  if (entityId())
  { 
    // Contents page
    ui_.ictDateEdit->setDate(entityData_.ictDate);      
    ui_.bankEdit->setText(bankShortName_);
    selectedBankId_ = entityData_.bankId;
    if (entityData_.state)
    {
      ui_.stateCheck->setCheckState(Qt::Checked);
      ui_.recvDateEdit->setDate(entityData_.recvDate.get());
    }
    else 
    {
      ui_.stateCheck->setCheckState(Qt::Unchecked);
    }
    populatePaysheetTable(paysheetTableData_);
    nonPersistPaysheetTableData_ = paysheetTableData_;
    // General data page
    ui_.createUserEdit->setText(createUserName_);
    ui_.createTimeEdit->setText(entityData_.createTime.toString(
        Qt::DefaultLocaleShortDate));
    ui_.updateUserEdit->setText(updateUserName_);
    ui_.updateTimeEdit->setText(entityData_.updateTime.toString(
        Qt::DefaultLocaleShortDate));

    contentsChanged_ = false;
  }
}    

bool IctCardPage::updateDatabase()
{
  static const char* querySql = "update \"ICT\" set " \
      "\"ICT_DATE\" = ?, \"BANK_ID\" = ?, \"STATE\" = ?, \"RECV_DATE\" = ?, " \
      "\"REMARK\" = ?, " \
      "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\' " \
      "where \"ICT_ID\" = ?";

  static const char* removePaysheetQuerySql = 
      "delete from \"PAYSHEET_ICT_RELATION\" where \"ICT_ID\" = ?";

  static const char* addPaysheetQuerySql = "insert into " \
      "\"PAYSHEET_ICT_RELATION\"(\"ICT_ID\", \"PAY_SHEET_ID\", \"TAB_ORDER\") " \
      "values(?, ?, ?)";

  if (entityId())
  {        
    QSqlDatabase database(databaseModel()->database());
    if (database.transaction())
    {          
      QSqlQuery query(database);
      QSqlQuery removePaysheetQuery(database);
      QSqlQuery insertPaysheetQuery(database);
      if (query.prepare(querySql) 
          && removePaysheetQuery.prepare(removePaysheetQuerySql) 
          && insertPaysheetQuery.prepare(addPaysheetQuerySql))
      {
        query.bindValue(0, entityData_.ictDate);
        query.bindValue(1, entityData_.bankId);
        query.bindValue(2, entityData_.state);
        query.bindValue(3, entityData_.recvDate ? entityData_.recvDate.get()
            : QVariant(QVariant::Date));
        query.bindValue(4, entityData_.remark ? entityData_.remark.get() 
            : QVariant(QVariant::String));
        query.bindValue(5, databaseModel()->userId());
        query.bindValue(6, entityData_.ictId);
        if (query.exec())
        {
          if (0 < query.numRowsAffected())
          {
            removePaysheetQuery.bindValue(0, entityData_.ictId);
            if (removePaysheetQuery.exec())
            {                  
              if (insertPaysheetData(insertPaysheetQuery, 
                  entityData_.ictId, paysheetTableData_))
              {
                if (database.commit())
                {
                  databaseModel()->notifyUpdate(entityData_);
                  return true;
                }
                else
                {
                  database.rollback();          
                }
              } // if all ict's paysheets inserted
            } // if all ict's paysheets removed
          } // if any record was updated
        } // if ict updated
      } // if prepare queries
      database.rollback();          
    } // if (database.transaction())
  } // if (entityId())
  return false;
}

bool IctCardPage::insertIntoDatabase()
{
  static const char* querySql = "insert into " \
      "\"ICT\"(\"ICT_ID\", \"ICT_DATE\", \"BANK_ID\", \"STATE\", \"RECV_DATE\", " \
      "\"REMARK\", \"CREATE_USER_ID\", \"CREATE_TIME\", \"UPDATE_USER_ID\", \"UPDATE_TIME\") " \
      "values(?, ?, ?, ?, ?, ?, ?, 'NOW', ?, 'NOW')";
      
  static const char* addPaysheetQuerySql = "insert into " \
      "\"PAYSHEET_ICT_RELATION\"(\"ICT_ID\", \"PAY_SHEET_ID\", \"TAB_ORDER\") " \
      "values(?, ?, ?)";

  if (OptionalQInt64 newEntityId = databaseModel()->generateId(ictEntity))
  {      
    entityData_.ictId = newEntityId.get();
    QSqlDatabase database(databaseModel()->database());
    if (database.transaction())
    {          
      QSqlQuery query(database);          
      QSqlQuery insertPaysheetQuery(database);
      if (query.prepare(querySql) 
          && insertPaysheetQuery.prepare(addPaysheetQuerySql))
      {
        query.bindValue(0, entityData_.ictId);            
        query.bindValue(1, entityData_.ictDate);
        query.bindValue(2, entityData_.bankId);
        query.bindValue(3, entityData_.state);
        query.bindValue(4, entityData_.recvDate ? entityData_.recvDate.get()
            : QVariant(QVariant::Date));          
        query.bindValue(5, entityData_.remark ? entityData_.remark.get()
            : QVariant(QVariant::String));          
        query.bindValue(6, databaseModel()->userId());
        query.bindValue(7, databaseModel()->userId());
        if (query.exec())
        {
          if (0 < query.numRowsAffected())
          {                
            if (insertPaysheetData(insertPaysheetQuery, 
                entityData_.ictId, paysheetTableData_))
            {
              if (database.commit())
              {
                setEntityId(newEntityId.get());
                databaseModel()->notifyInsert(entityData_);
                return true;
              }
              else
              {
                database.rollback();          
              }
            } // if all ict's paysheets inserted                
          } // if any record was inserted
        } // if ict updated
      } // if prepare queries
      database.rollback();          
    } // if (database.transaction())
  } // if newEntityId
  return false;
}

bool IctCardPage::removeFromDatabase()
{
  static const char* querySql = 
      "delete from \"ICT\" where \"ICT_ID\" = ?";
  static const char* paysheetQuerySql = 
      "delete from \"PAYSHEET_ICT_RELATION\" where \"ICT_ID\" = ?";

  if (QMessageBox::Yes == QMessageBox::question(this, 
      tr("Inquiries on currency transactions"), 
      tr("Delete inquiry on currency transactions?"), 
      QMessageBox::Yes | QMessageBox::No, 
      QMessageBox::No))
  {
    if (entityId())
    {
      QSqlDatabase database(databaseModel()->database());
      if (database.transaction())
      {          
        QSqlQuery query(database);   
        QSqlQuery paysheetQuery(database);               
        if (query.prepare(querySql) && paysheetQuery.prepare(paysheetQuerySql))
        {              
          paysheetQuery.bindValue(0, entityData_.ictId);
          if (paysheetQuery.exec())
          {
            query.bindValue(0, entityData_.ictId);
            if (query.exec())
            {
              if (0 < query.numRowsAffected())
              {
                if (database.commit())
                {
                  databaseModel()->notifyRemove(entityData_);
                  return true;
                }
                else
                {
                  database.rollback();
                  return false;
                }                    
              }            
            }
          }              
        }
        database.rollback();
      } // if (database.transaction())
    } // if (entityId())
  }
  return false;
}

void IctCardPage::on_lineEdit_textEdited(const QString&)
{
  contentsChanged_ = true;
  updateWidgets();
}

void IctCardPage::on_textEdit_textChanged()
{
  contentsChanged_ = true;
  updateWidgets();
}

void IctCardPage::on_dateEdit_dateChanged(const QDate&)
{
  contentsChanged_ = true;
  updateWidgets();
}    

void IctCardPage::on_bankBtn_clicked(bool /*checked*/)
{
  if (viewMode == mode())
  {
    if (entityId())
    {          
      showBankCardTaskWindow(entityData_.bankId);
    }        
  }
  else
  {
    //todo: add default selection
    std::auto_ptr<ListPage> listPage(new BankListPage(resourceManager(), 
        taskWindowManager(), databaseModel(), helpAction(),  
        UserDefinedFilter(), UserDefinedOrder(), 0));
    ListSelectDialog listSelectDialog(this, listPage);
    listSelectDialog.setWindowModality(Qt::WindowModal);
    if (QDialog::Accepted == listSelectDialog.exec())
    {          
      selectedBankId_ = listSelectDialog.listPage()->selectedId();          
      if (OptionalQString bankShortName = 
          listSelectDialog.listPage()->selectedRelationalText())
      {
        ui_.bankEdit->setText(bankShortName.get());
      }
      contentsChanged_ = true;
      updateWidgets();
    }
  } 
}

void IctCardPage::on_addPaysheetBtn_clicked(bool /*checked*/)
{
  if (viewMode != mode())
  {        
    std::auto_ptr<ListPage> listPage(new PaysheetListPage(resourceManager(), 
        taskWindowManager(), databaseModel(), helpAction(), OptionalInt(), 
        UserDefinedFilter(), UserDefinedOrder(), 0));
    ListSelectDialog listSelectDialog(this, listPage);
    listSelectDialog.setWindowModality(Qt::WindowModal);
    if (QDialog::Accepted == listSelectDialog.exec())
    {          
      if (OptionalQInt64 selectedPaysheetId = 
          listSelectDialog.listPage()->selectedId())
      {
        if (addPaysheet(selectedPaysheetId.get()))
        {
          contentsChanged_ = true;
          updateWidgets();
        }
      }          
    }
  } 
}

void IctCardPage::on_removePaysheetBtn_clicked(bool /*checked*/)
{
  if (viewMode != mode())
  {        
    int row = ui_.paysheetTable->currentRow();
    if (row >= 0)
    {
      nonPersistPaysheetTableData_.erase(
          nonPersistPaysheetTableData_.begin() + row);
      populatePaysheetTable(nonPersistPaysheetTableData_);
      contentsChanged_ = true;
      updateWidgets();
    }
  }   
}

void IctCardPage::on_moveUpBtn_clicked(bool /*checked*/)
{
  if (viewMode != mode())
  {        
    int row = ui_.paysheetTable->currentRow();
    if (0 < row)
    {
      int previousRow = row - 1;
      int columnCount = ui_.paysheetTable->columnCount();
      for (int column = 0; column != columnCount; ++column)
      {
        std::auto_ptr<QTableWidgetItem> previousItem(
            ui_.paysheetTable->takeItem(previousRow, column));
        std::auto_ptr<QTableWidgetItem> item(
            ui_.paysheetTable->takeItem(row, column));
        ui_.paysheetTable->setItem(previousRow, column, item.release());
        ui_.paysheetTable->setItem(row, column, previousItem.release());
      }

      PaysheetData paysheetData = nonPersistPaysheetTableData_[previousRow];
      nonPersistPaysheetTableData_[previousRow] = 
          nonPersistPaysheetTableData_[row];
      nonPersistPaysheetTableData_[row] = paysheetData;

      ui_.paysheetTable->setCurrentCell(
          previousRow, ui_.paysheetTable->currentColumn());
      contentsChanged_ = true;
      updateWidgets();
    }        
  }   
}

void IctCardPage::on_moveDownBtn_clicked(bool /*checked*/)
{
  if (viewMode != mode())
  {        
    int row = ui_.paysheetTable->currentRow();
    if (0 <= row)
    {
      int rowCount = ui_.paysheetTable->rowCount();
      if (row < rowCount - 1)
      {
        int nextRow = row + 1;
        int columnCount = ui_.paysheetTable->columnCount();
        for (int column = 0; column != columnCount; ++column)
        {
          std::auto_ptr<QTableWidgetItem> nextItem(
              ui_.paysheetTable->takeItem(nextRow, column));
          std::auto_ptr<QTableWidgetItem> item(
              ui_.paysheetTable->takeItem(row, column));
          ui_.paysheetTable->setItem(nextRow, column, item.release());
          ui_.paysheetTable->setItem(row, column, nextItem.release());
        }

        PaysheetData paysheetData = nonPersistPaysheetTableData_[nextRow];
        nonPersistPaysheetTableData_[nextRow] = 
            nonPersistPaysheetTableData_[row];
        nonPersistPaysheetTableData_[row] = paysheetData;

        ui_.paysheetTable->setCurrentCell(
            nextRow, ui_.paysheetTable->currentColumn());
        contentsChanged_ = true;
        updateWidgets();
      }          
    }        
  }   
}

bool IctCardPage::addPaysheet(qint64 paysheetId)
{
  int rowCount = ui_.paysheetTable->rowCount();
  if (rowCount < maxPayheets)
  {        
    if (!contains(nonPersistPaysheetTableData_, paysheetId))
    {
      if (boost::optional<PaysheetData> paysheetData = 
          readPaysheetData(paysheetId))
      {
        nonPersistPaysheetTableData_.push_back(*paysheetData);
        populatePaysheetTable(nonPersistPaysheetTableData_);
        return true;
      }                    
    }        
  }
  return false;
}

void IctCardPage::on_paysheetTable_currentCellChanged(int /*currentRow*/, 
    int /*currentColumn*/, int /*previousRow*/, int /*previousColumn*/)
{
  updatePaysheetTableWidgets();
}

void IctCardPage::on_paysheetTable_cellDoubleClicked(int row, int /*column*/)
{
  if (row >= 0 && row < nonPersistPaysheetTableData_.size())
  {
    showPaysheetCardTaskWindow(
        nonPersistPaysheetTableData_[row].paysheet.paysheetId);
  }      
}

void IctCardPage::on_paysheetTable_headerContextMenu_requested(
    const QPoint& pos)
{
  QHeaderView* header = qobject_cast<QHeaderView*>(sender());
  if (header)
  {        
    if (header->count() > 1)
    {
      QMenu contextMenu;
      Qt::Orientation headerOrientation = header->orientation();
      QAbstractItemModel* model = header->model();
      bool loneVisible = header->count() - header->hiddenSectionCount() < 2;
      for (int i = 0, count = header->count(); i != count; ++i)
      {
        int section = header->logicalIndex(i);           
        QVariant sectionData = model->headerData(section, headerOrientation);
        QString sectionCaption = sectionData.value<QString>();
        QAction* sectionAction = contextMenu.addAction(sectionCaption);
        sectionAction->setCheckable(true);
        bool visible = !header->isSectionHidden(section);
        sectionAction->setChecked(visible);
        if (loneVisible && visible)
        {
          sectionAction->setEnabled(false);
        }
      }        
      if (!contextMenu.isEmpty())
      {
        QAction* action = contextMenu.exec(header->mapToGlobal(pos));
        int index = contextMenu.actions().indexOf(action);            
        if (0 <= index)
        {              
          int section = header->logicalIndex(index);              
          header->setSectionHidden(section, !header->isSectionHidden(section));
        }
      }
    }
  }
}

void IctCardPage::on_stateCheck_stateChanged(int /*state*/)
{
  contentsChanged_ = true;
  updateWidgets();
}

IctCardPage::PaysheetDataVector IctCardPage::readPaysheetData(
    QSqlQuery& query) const
{
  PaysheetDataVector vector;
  while (query.next())
  {
    PaysheetData data;
    data.paysheet.paysheetId = query.value(0).toLongLong();
    data.paysheet.contractId = query.value(1).toLongLong();
    data.contractNumber      = query.value(2).toString();
    data.paysheet.ccdId      = query.value(3).isNull() 
        ? OptionalQInt64() : query.value(3).toLongLong();
    data.ccdNumber           = query.value(4).isNull() 
        ? OptionalQString() : query.value(4).toString();
    data.paysheet.docNumber  = query.value(5).toString();
    data.paysheet.payDate    = 
        databaseModel()->convertFromServer(query.value(6).toDate());
    data.paysheet.amount     = query.value(7).toLongLong();
    data.currencyAlphabeticCode  = query.value(8).toString();
    data.paysheet.accountNumber  = query.value(9).isNull() 
        ? OptionalQString() : query.value(9).toString();
    data.paysheet.remark         = query.isNull(10) 
        ? OptionalQString() : query.value(10).toString();
    data.paysheet.createUserId   = query.value(11).toLongLong();
    data.paysheet.updateUserId   = query.value(12).toLongLong();        
    data.paysheet.createTime = databaseModel()->convertFromServer(
        query.value(15).toDateTime()); 
    data.paysheet.updateTime = databaseModel()->convertFromServer(
        query.value(16).toDateTime());              
    data.paysheet.currencyId = query.value(17).toLongLong();
    data.passportNumber = query.value(18).isNull()
        ? OptionalQString() : query.value(18).toString();
    vector.append(data);
  }      
  return vector;
}

bool IctCardPage::insertPaysheetData(QSqlQuery& query, qint64 ictId, 
    const PaysheetDataVector& paysheetData) const
{
  typedef PaysheetDataVector::const_iterator iterator;      
  int tabOrder = 0;
  query.bindValue(0, ictId);
  for (iterator i = paysheetData.begin(), end = paysheetData.end(); 
      i != end; ++i)
  {
    query.bindValue(1, i->paysheet.paysheetId);
    query.bindValue(2, tabOrder);
    if (!query.exec())
    {
      return false;
    }
    ++tabOrder;                    
  }
  return true;
}

bool IctCardPage::contains(const PaysheetDataVector& paysheetData, 
    qint64 paysheetId)
{
  typedef PaysheetDataVector::const_iterator iterator;      
  for (iterator i = paysheetData.begin(), end = paysheetData.end(); 
      i != end; ++i)
  {
    if (i->paysheet.paysheetId == paysheetId)
    {
      return true;
    }
  }
  return false;
}

bool IctCardPage::contains(const PaysheetDataVector& paysheetData, 
    const ma::chrono::model::Paysheet& paysheet)
{
  return contains(paysheetData, paysheet.paysheetId);
}

bool IctCardPage::contains(const PaysheetDataVector& paysheetData, 
    const ma::chrono::model::Currency& currency)
{
  typedef PaysheetDataVector::const_iterator iterator;      
  for (iterator i = paysheetData.begin(), end = paysheetData.end(); 
      i != end; ++i)
  {
    if (i->paysheet.currencyId == currency.currencyId)
    {
      return true;
    }
  }
  return false;
}

bool IctCardPage::contains(const PaysheetDataVector& paysheetData, 
    const ma::chrono::model::Contract& contract)
{
  typedef PaysheetDataVector::const_iterator iterator;      
  for (iterator i = paysheetData.begin(), end = paysheetData.end(); 
      i != end; ++i)
  {
    if (i->paysheet.contractId == contract.contractId)
    {
      return true;
    }
  }
  return false;
}

bool IctCardPage::contains(const PaysheetDataVector& paysheetData, 
    const ma::chrono::model::Ccd& ccd)
{
  typedef PaysheetDataVector::const_iterator iterator;      
  for (iterator i = paysheetData.begin(), end = paysheetData.end(); 
      i != end; ++i)
  {
    if (i->paysheet.ccdId)
    {        
      if (i->paysheet.ccdId.get() == ccd.ccdId)
      {
        return true;
      }
    }
  }
  return false;
}

void IctCardPage::update(PaysheetDataVector& paysheetData, 
    const PaysheetData& updatedPaysheetData)
{
  typedef PaysheetDataVector::iterator iterator;      
  for (iterator i = paysheetData.begin(), end = paysheetData.end(); 
      i != end; ++i)
  {
    if (i->paysheet.paysheetId == updatedPaysheetData.paysheet.paysheetId)
    {
      *i = updatedPaysheetData;
    }
  }      
}

void IctCardPage::update(PaysheetDataVector& paysheetData, 
    const ma::chrono::model::Currency& currency)
{
  typedef PaysheetDataVector::iterator iterator;      
  for (iterator i = paysheetData.begin(), end = paysheetData.end(); 
      i != end; ++i)
  {
    if (i->paysheet.currencyId == currency.currencyId)
    {
      i->currencyAlphabeticCode = currency.alphabeticCode;          
    }
  }              
}

void IctCardPage::update(PaysheetDataVector& paysheetData, 
    const ma::chrono::model::Contract& contract)
{
  typedef PaysheetDataVector::iterator iterator;      
  for (iterator i = paysheetData.begin(), end = paysheetData.end(); 
      i != end; ++i)
  {
    if (i->paysheet.contractId == contract.contractId)
    {
      i->contractNumber = contract.number;          
      i->passportNumber = contract.passportNumber;
    }
  }              
}

void IctCardPage::update(PaysheetDataVector& paysheetData, 
    const ma::chrono::model::Ccd& ccd)
{
  typedef PaysheetDataVector::iterator iterator;      
  for (iterator i = paysheetData.begin(), end = paysheetData.end(); 
      i != end; ++i)
  {
    if (i->paysheet.ccdId)
    {        
      if (i->paysheet.ccdId.get() == ccd.ccdId)
      {
        i->ccdNumber = ccd.number;          
      }
    }
  }
}

void IctCardPage::remove(PaysheetDataVector& paysheetData, qint64 paysheetId)
{
  typedef PaysheetDataVector::iterator iterator;      
  iterator i = paysheetData.begin();
  iterator end = paysheetData.end();
  while (i != end)      
  {
    if (i->paysheet.paysheetId == paysheetId)
    {
      i = paysheetData.erase(i);
      end = paysheetData.end();
    }
    else
    {
      ++i;
    }        
  }
}

boost::optional<IctCardPage::PaysheetData> IctCardPage::readPaysheetData(
    qint64 paysheetId) const
{
  static const char* querySql = "select " \
      "t.\"CONTRACT_ID\", cr.\"NUMBER\", " \
      "t.\"CCD_ID\", c.\"NUMBER\", " \
      "t.\"DOC_NUMBER\", t.\"PAY_DATE\", t.\"AMOUNT\", " \
      "cur.\"ALPHABETIC_CODE\", t.\"ACCOUNT_NUMBER\", t.\"REMARK\", " \
      "t.\"CREATE_USER_ID\", t.\"UPDATE_USER_ID\", " \
      "cu.\"LOGIN\", uu.\"LOGIN\", " \
      "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", t.\"CURRENCY_ID\", " \
      "cr.\"PASSPORT_NUMBER\" " \
      "from \"PAY_SHEET\" t " \
      "join \"CONTRACT\" cr on t.\"CONTRACT_ID\" = cr.\"CONTRACT_ID\" " \
      "left join \"CCD\" c on t.\"CCD_ID\" = c.\"CCD_ID\" " \
      "join \"CURRENCY\" cur on t.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
      "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
      "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
      "where t.\"PAY_SHEET_ID\" = ?";

  QSqlDatabase database(databaseModel()->database());        
  QSqlQuery query(database);      
  if (query.prepare(querySql))
  {
    query.bindValue(0, paysheetId);        
    if (query.exec())
    {
      if (query.next())
      {          
        PaysheetData paysheetData;
        paysheetData.paysheet.paysheetId = paysheetId;
        paysheetData.paysheet.contractId = query.value(0).toLongLong();
        paysheetData.contractNumber      = query.value(1).toString();
        paysheetData.paysheet.ccdId      = query.value(2).isNull() 
            ? OptionalQInt64() : query.value(2).toLongLong();
        paysheetData.ccdNumber           = query.value(3).isNull() 
            ? OptionalQString() : query.value(3).toString();
        paysheetData.paysheet.docNumber  = query.value(4).toString();
        paysheetData.paysheet.payDate    = databaseModel()->convertFromServer(
            query.value(5).toDate());
        paysheetData.paysheet.amount     = query.value(6).toLongLong();
        paysheetData.currencyAlphabeticCode  = query.value(7).toString();
        paysheetData.paysheet.accountNumber  = query.value(8).isNull() 
            ? OptionalQString() : query.value(8).toString();              
        paysheetData.paysheet.remark         = query.isNull(9) 
            ? OptionalQString() : query.value(9).toString();
        paysheetData.paysheet.createUserId   = query.value(10).toLongLong();
        paysheetData.paysheet.updateUserId   = query.value(11).toLongLong();        
        paysheetData.paysheet.createTime = databaseModel()->convertFromServer(
            query.value(14).toDateTime());
        paysheetData.paysheet.updateTime = databaseModel()->convertFromServer(
            query.value(15).toDateTime());
        paysheetData.paysheet.currencyId = query.value(16).toLongLong();
        paysheetData.passportNumber = query.value(17).isNull() 
            ? OptionalQString() : query.value(17).toString();
        return paysheetData;
      }
    }
  }      
  return boost::optional<PaysheetData>();
}    

void IctCardPage::populatePaysheetTable(const PaysheetDataVector& vector)
{      
  typedef PaysheetDataVector::const_iterator iterator;      
  static const QVariant leftAlignment = QVariant(
      Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
  static const QVariant rightAlignment = QVariant(
      Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
  int row = 0;      
  ui_.paysheetTable->setRowCount(vector.size());
  for (iterator i = vector.begin(), end = vector.end(); i != end; ++i)
  {
    std::auto_ptr<QTableWidgetItem> item(
        new QTableWidgetItem(i->paysheet.docNumber));
    item->setData(Qt::TextAlignmentRole, rightAlignment);                
    ui_.paysheetTable->setItem(row, 0, item.release());

    item.reset(new QTableWidgetItem(i->paysheet.accountNumber 
        ? i->paysheet.accountNumber.get() : QString()));
    item->setData(Qt::TextAlignmentRole, rightAlignment);                
    ui_.paysheetTable->setItem(row, 1, item.release());

    item.reset(new QTableWidgetItem(
        i->paysheet.payDate.toString(Qt::DefaultLocaleShortDate)));
    item->setData(Qt::TextAlignmentRole, leftAlignment);                
    ui_.paysheetTable->setItem(row, 2, item.release());

    item.reset(new QTableWidgetItem(
        model::CurrencyAmount(i->paysheet.amount).toString()));
    item->setData(Qt::TextAlignmentRole, rightAlignment);                
    ui_.paysheetTable->setItem(row, 3, item.release());

    item.reset(new QTableWidgetItem(i->currencyAlphabeticCode));
    item->setData(Qt::TextAlignmentRole, leftAlignment);                
    ui_.paysheetTable->setItem(row, 4, item.release());

    item.reset(new QTableWidgetItem(i->contractNumber));
    item->setData(Qt::TextAlignmentRole, rightAlignment);                
    ui_.paysheetTable->setItem(row, 5, item.release());

    item.reset(new QTableWidgetItem(
        i->passportNumber ? i->passportNumber.get() : QString()));
    item->setData(Qt::TextAlignmentRole, rightAlignment);
    ui_.paysheetTable->setItem(row, 6, item.release());

    item.reset(new QTableWidgetItem(i->ccdNumber 
        ? i->ccdNumber.get() : QString()));
    item->setData(Qt::TextAlignmentRole, rightAlignment);                
    ui_.paysheetTable->setItem(row, 7, item.release());

    ++row;        
  }      
}    

void IctCardPage::showRequiredMarkers(bool visible)
{
  WidgetUtility::show(requiredMarkers_, visible);
}

void IctCardPage::updatePaysheetTableWidgets()
{
  if (viewMode == mode()) 
  {
    ui_.addPaysheetBtn->setEnabled(false);
    ui_.removePaysheetBtn->setEnabled(false);
    ui_.moveUpBtn->setEnabled(false);
    ui_.moveDownBtn->setEnabled(false);
  }
  else
  {
    int currentRow = ui_.paysheetTable->currentRow();
    int rowCount = ui_.paysheetTable->rowCount();
    ui_.addPaysheetBtn->setEnabled(rowCount < maxPayheets);
    ui_.removePaysheetBtn->setEnabled(rowCount > 0 && currentRow >= 0);
    ui_.moveUpBtn->setEnabled(currentRow > 0);
    ui_.moveDownBtn->setEnabled(currentRow >= 0 && currentRow < rowCount - 1);
  }      
}

void IctCardPage::on_createUserBtn_clicked(bool /*checked*/)
{
  if (entityId())
  {      
    showUserCardTaskWindow(entityData_.createUserId);
  }        
}

void IctCardPage::on_updateUserBtn_clicked(bool /*checked*/)
{
  if (entityId())
  {      
    showUserCardTaskWindow(entityData_.updateUserId);
  }        
}

void IctCardPage::on_database_userUpdated(const ma::chrono::model::User& user)
{
  bool updated = false;
  if (entityId())
  {        
    if (entityData_.createUserId == user.userId)
    {
      createUserName_ = user.userName;
      ui_.createUserEdit->setText(createUserName_);
      updated = true;
    }
    if (entityData_.updateUserId == user.userId)
    {
      updateUserName_ = user.userName;
      ui_.updateUserEdit->setText(updateUserName_);
      updated = true;
    }
    if (updated)
    {         
      updateWidgets();
    }
  }
}

void IctCardPage::on_database_bankUpdated(const ma::chrono::model::Bank& bank)
{
  if (entityId())
  {        
    if (entityData_.bankId == bank.bankId)
    {
      bankCode_ = bank.bankCode;
      bankShortName_ = bank.shortName;
    }        
  }
  if (selectedBankId_)
  {
    if (selectedBankId_.get() == bank.bankId)
    {          
      ui_.bankEdit->setText(bank.shortName);
      updateWidgets();
    }
  }
}

void IctCardPage::on_database_bankRemoved(const ma::chrono::model::Bank& bank)
{
  if (selectedBankId_)
  {
    if (selectedBankId_.get() == bank.bankId)
    {          
      ui_.bankEdit->setText(QString());
      selectedBankId_.reset();
      updateWidgets();
    }
  }
}

void IctCardPage::on_database_paysheetUpdated(
    const ma::chrono::model::Paysheet& paysheet)
{
  boost::optional<PaysheetData> updatedPaysheetData;
  if (contains(paysheetTableData_, paysheet))
  {
    updatedPaysheetData = readPaysheetData(paysheet.paysheetId);
    if (updatedPaysheetData)
    {
      update(paysheetTableData_, *updatedPaysheetData);
    }
  }
  if (contains(nonPersistPaysheetTableData_, paysheet))
  {
    if (!updatedPaysheetData)
    {
      updatedPaysheetData = readPaysheetData(paysheet.paysheetId);
    }
    if (updatedPaysheetData)
    {
      update(nonPersistPaysheetTableData_, *updatedPaysheetData);
      populatePaysheetTable(nonPersistPaysheetTableData_);
      updateWidgets();
    }
  }      
}

void IctCardPage::on_database_paysheetRemoved(
    const ma::chrono::model::Paysheet& paysheet)
{
  boost::optional<PaysheetData> updatedPaysheetData;
  if (contains(paysheetTableData_, paysheet))
  {
    remove(paysheetTableData_, paysheet.paysheetId);        
  }
  if (contains(nonPersistPaysheetTableData_, paysheet))
  {
    remove(nonPersistPaysheetTableData_, paysheet.paysheetId);
    populatePaysheetTable(nonPersistPaysheetTableData_);
    updateWidgets();
  }      
}

void IctCardPage::on_database_currencyUpdated(
    const ma::chrono::model::Currency& currency)
{      
  if (contains(paysheetTableData_, currency))
  {        
    update(paysheetTableData_, currency);        
  }
  if (contains(nonPersistPaysheetTableData_, currency))
  {
    update(nonPersistPaysheetTableData_, currency);
    populatePaysheetTable(nonPersistPaysheetTableData_);
    updateWidgets();
  }
}

void IctCardPage::on_database_contractUpdated(
    const ma::chrono::model::Contract& contract)
{      
  if (contains(paysheetTableData_, contract))
  {        
    update(paysheetTableData_, contract);
  }
  if (contains(nonPersistPaysheetTableData_, contract))
  {
    update(nonPersistPaysheetTableData_, contract);
    populatePaysheetTable(nonPersistPaysheetTableData_);
    updateWidgets();
  }
}

void IctCardPage::on_database_ccdUpdated(const ma::chrono::model::Ccd& ccd)
{      
  if (contains(paysheetTableData_, ccd))
  {        
    update(paysheetTableData_, ccd);
  }
  if (contains(nonPersistPaysheetTableData_, ccd))
  {
    update(nonPersistPaysheetTableData_, ccd);
    populatePaysheetTable(nonPersistPaysheetTableData_);
    updateWidgets();
  }
}           

static QString findTemplate(int bankCode)
{
  return QCoreApplication::applicationDirPath() 
      + QString::fromWCharArray(L"/ict/%1.xlt").arg(bankCode);
}

static bool checkTemplateExistance(const QString& filename)
{
  return QFile::exists(filename);
}

void IctCardPage::on_sendToExcelAction_triggered()
{
  QString templateFilename = findTemplate(bankCode_);
  if (!checkTemplateExistance(templateFilename))
  {
    //todo
    return;
  }
  ExcelExporter::exportIct(entityData_.ictId, 
      templateFilename, *databaseModel());
  //todo
}

} // namespace chrono
} // namespace ma
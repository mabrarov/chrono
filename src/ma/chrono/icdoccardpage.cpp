/*
TRANSLATOR ma::chrono::IcdocCardPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
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
#include <ma/chrono/ccdlistpage.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/widgetutility.h>
#include <ma/chrono/sqlutility.h>
#include <ma/chrono/model/bank.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/currencyamount.h>
#include <ma/chrono/model/currency.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/ccd.h>
#include <ma/chrono/localeutility.h>
#include <ma/chrono/excelexporter.h>
#include <ma/chrono/icdoccardpage.h>

namespace ma {

namespace chrono {

namespace {
  
  const int maxPayheets = 30;

}    

IcdocCardPage::IcdocCardPage(const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerWeakPtr& taskWindowManager,
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, Mode mode, 
    const OptionalQInt64& entityId, QWidget* parent)
  : CardPage(resourceManager, taskWindowManager, databaseModel, 
        helpAction, icdocEntity, mode, entityId, parent)
  , dataAwareWidgets_()
  , contentsChanged_(false)     
{
  ui_.setupUi(this);          
  ui_.tabWidget->setCurrentIndex(0);
  setWindowIcon(resourceManager->getIcon(chronoIconName));
  LocaleUtility::setupLocaleAwareWidget(ui_.icdocDateEdit);
  LocaleUtility::setupLocaleAwareWidget(ui_.recvDateEdit);            
  ui_.moveUpBtn->setIcon(resourceManager->getIcon(moveUpIconName));
  ui_.moveDownBtn->setIcon(resourceManager->getIcon(moveDownIconName));            
  createAction()->setText(tr("&New inquiry on confirming documents"));
  dataAwareWidgets_ << ui_.icdocDateEdit 
                    << ui_.recvDateEdit 
                    << ui_.stateCheck 
                    << ui_.remarksEdit;
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

  QHeaderView* ccdTableHeader = ui_.ccdTable->horizontalHeader();

  // Qt 5 migration
#if QT_VERSION >= 0x050000      
  ccdTableHeader->setSectionsMovable(true);
#else
  ccdTableHeader->setMovable(true);
#endif

  ccdTableHeader->setContextMenuPolicy(Qt::CustomContextMenu);
  ccdTableHeader->setHighlightSections(false);      
  ccdTableHeader->setStretchLastSection(true);
  QObject::connect(ccdTableHeader, 
      SIGNAL(customContextMenuRequested(const QPoint&)), 
      SLOT(on_ccdTable_headerContextMenu_requested(const QPoint&)));      
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
      SIGNAL(ccdUpdated(const ma::chrono::model::Ccd&)), 
      SLOT(on_database_ccdUpdated(const ma::chrono::model::Ccd&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(ccdRemoved(const ma::chrono::model::Ccd&)),
      SLOT(on_database_ccdRemoved(const ma::chrono::model::Ccd&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(currencyUpdated(const ma::chrono::model::Currency&)), 
      SLOT(on_database_currencyUpdated(const ma::chrono::model::Currency&)));
  QObject::connect(databaseModel.get(),
      SIGNAL(contractUpdated(const ma::chrono::model::Contract&)),
      SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));
  updateWidgets();         
}

IcdocCardPage::~IcdocCardPage()
{
} 

void IcdocCardPage::refresh()
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

void IcdocCardPage::save()
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

void IcdocCardPage::cancel()
{
  if (editMode == mode())
  {      
    populateWidgets();
  }      
  CardPage::cancel();
  updateWidgets();
}

void IcdocCardPage::edit()
{      
  CardPage::edit();
  updateWidgets();
}

void IcdocCardPage::remove()
{
  if (createMode != mode())
  {
    if (removeFromDatabase())
    {
      CardPage::remove();
    }
  }            
}

std::auto_ptr<CardPage> IcdocCardPage::createCardPage(CardPage::Mode mode,
    const OptionalQInt64& entityId) const
{
  std::auto_ptr<CardPage> cardPage(new IcdocCardPage(resourceManager(), 
      taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
  return cardPage;
}

void IcdocCardPage::connectDataAwareWidgets()
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
          SIGNAL(textChanged()),
          SLOT(on_textEdit_textChanged()));
    }        
    else if (QDateEdit* edit = qobject_cast<QDateEdit*>(widget))
    {
      QObject::connect(edit, 
          SIGNAL(dateChanged(const QDate&)), 
          SLOT(on_dateEdit_dateChanged(const QDate&)));
    }
  }
}

void IcdocCardPage::updateWidgets()
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
      updateWindowTitle(tr("Inquiries on confirming documents - By %1 for %2")
          .arg(entityData_.icdocDate.toString(Qt::DefaultLocaleShortDate))
          .arg(bankShortName_));
    }
    ui_.ccdTableModifyPanel->setVisible(false);        
    showRequiredMarkers(false);
    break;

  case editMode:
    if (entityId())
    {
      if (contentsChanged_)
      {
        updateWindowTitle(tr("Inquiries on confirming documents - By %1 for %2* - edit")
            .arg(entityData_.icdocDate.toString(Qt::DefaultLocaleShortDate))
            .arg(bankShortName_));            
      }
      else
      {
        updateWindowTitle(tr("Inquiries on confirming documents - By %1 for %2 - edit")
            .arg(entityData_.icdocDate.toString(Qt::DefaultLocaleShortDate))
            .arg(bankShortName_));            
      }        
    }
    ui_.ccdTableModifyPanel->setVisible(true);        
    showRequiredMarkers(true);
    break;

  case createMode:        
    updateWindowTitle(tr("Inquiries on confirming documents - New inquiry on confirming documents*"));
    ui_.ccdTableModifyPanel->setVisible(true);
    showRequiredMarkers(true);
    break;
  }
  bool confirmed = Qt::Checked == ui_.stateCheck->checkState();
  ui_.recvDateLabel->setVisible(confirmed);
  ui_.recvDateEdit->setVisible(confirmed);      
  updateCcdTableWidgets();
}

bool IcdocCardPage::populateFromWidgets()
{
  static const char* bankTitle = QT_TR_NOOP("Bank");            
  static const char* ccdTableTitle = QT_TR_NOOP("CCD");            
  static const char* remarkTitle = QT_TR_NOOP("Remark");      
      
  static const char* messageTitle = QT_TR_NOOP("Invalid data input");
  static const char* notEmpty = QT_TR_NOOP("Field \"%1\" can\'t be empty.");
  static const char* listNotEmpty = QT_TR_NOOP("List \"%1\" can\'t be empty.");
  static const char* tooLongText = QT_TR_NOOP("Field \"%1\" can\'t exceed %2 characters.");      
            
  int contentsTabIndex = ui_.tabWidget->indexOf(ui_.contentsTab);      
  // Get bank
  if (!selectedBankId_)
  {        
    QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(bankTitle)));
    ui_.tabWidget->setCurrentIndex(contentsTabIndex);
    ui_.bankBtn->setFocus(Qt::TabFocusReason);
    return false;
  }  
  if (nonPersistCcdTableData_.isEmpty())
  {
    QMessageBox::warning(this, tr(messageTitle), tr(listNotEmpty).arg(tr(ccdTableTitle)));
    ui_.tabWidget->setCurrentIndex(contentsTabIndex);
    ui_.addCcdBtn->setFocus(Qt::TabFocusReason);
    return false;
  }      
  // Get remark      
  QString remark = ui_.remarksEdit->toPlainText().trimmed();
  if (remark.length() > model::Icdoc::remarkMaxLen)
  {
    int remarksTabIndex = ui_.tabWidget->indexOf(ui_.remarksTab);              
    QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(remarkTitle)).arg(model::Icdoc::remarkMaxLen));
    ui_.tabWidget->setCurrentIndex(remarksTabIndex);
    ui_.remarksEdit->setFocus(Qt::TabFocusReason);
    ui_.remarksEdit->selectAll();
    return false;
  }
  entityData_.icdocDate = ui_.icdocDateEdit->date();
  entityData_.bankId   = *selectedBankId_;
  entityData_.state    = Qt::Checked == ui_.stateCheck->checkState() ? 1 : 0;
  entityData_.recvDate = Qt::Checked == ui_.stateCheck->checkState() ? ui_.recvDateEdit->date() : OptionalQDate();      
  entityData_.remark = remark.isEmpty() ? OptionalQString() : remark;
  ccdTableData_ = nonPersistCcdTableData_;
  return true;
}

bool IcdocCardPage::populateFromDatabase()
{      
  static const char* querySql = "select " \
    "t.\"BANK_ID\", b.\"SHORT_NAME\", " \
    "t.\"ICDOC_DATE\", t.\"STATE\", t.\"RECV_DATE\", " \
    "t.\"REMARK\", " \
    "cu.\"LOGIN\", uu.\"LOGIN\", " \
    "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", " \
    "t.\"CREATE_USER_ID\", t.\"UPDATE_USER_ID\", b.\"BANK_CODE\" " \
    "from \"ICDOC\" t " \
    "join \"BANK\" b on t.\"BANK_ID\" = b.\"BANK_ID\" " \
    "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
    "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
    "where t.\"ICDOC_ID\" = ?";

  static const char* ccdQuerySql = "select " \
    "rel.\"CCD_ID\", " \
    "t.\"NUMBER\", t.\"CONTRACT_ID\", t.\"DEPPOINT_ID\", t.\"EXPORT_PERM_DATE\", " \
    "t.\"EXPORT_DATE\", t.\"PRODUCT_CODE\", t.\"SERIES\", t.\"TTERM_ID\", t.\"AMOUNT\", " \
    "t.\"DUTY\", t.\"RATE\", t.\"ZERO_TAX_PERIOD\", t.\"PAYMENT_TERM_TYPE\", " \
    "t.\"PAYMENT_TERM_DATE\", t.\"PAYMENT_TERM_PERIOD\", t.\"PAYMENT_LIMIT_DATE\", t.\"ZERO_TAX_LIMIT_DATE\", " \
    "t.\"CREATE_USER_ID\", t.\"UPDATE_USER_ID\", t.\"CREATE_TIME\", t.\"UPDATE_TIME\", " \
    "c.\"NUMBER\", c.\"PASSPORT_NUMBER\", c.\"CURRENCY_ID\", cur.\"ALPHABETIC_CODE\" " \
    "from \"CCD_ICDOC_RELATION\" rel " \
    "join \"CCD_SEL\" t on t.\"CCD_ID\" = rel.\"CCD_ID\" " \
    "join \"CONTRACT\" c on t.\"CONTRACT_ID\" = c.\"CONTRACT_ID\" " \
    "join \"CURRENCY\" cur on c.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
    "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
    "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
    "where rel.\"ICDOC_ID\" = ? order by rel.\"TAB_ORDER\"";

  if (entityId())
  {      
    QSqlDatabase database(databaseModel()->database());        
    if (database.transaction())
    {        
      QSqlQuery query(database);
      QSqlQuery ccdQuery(database);
      if (query.prepare(querySql) && ccdQuery.prepare(ccdQuerySql))
      {
        query.bindValue(0, entityId().get());
        ccdQuery.bindValue(0, entityId().get());
        if (query.exec() && ccdQuery.exec())
        {
          if (query.next())
          { 
            entityData_.icdocId = entityId().get();
            entityData_.bankId  = query.value(0).toLongLong();
            bankShortName_      = query.value(1).toString();
            entityData_.icdocDate = databaseModel()->convertFromServer(query.value(2).toDate());
            entityData_.state     = query.value(3).toInt();
            entityData_.recvDate  = query.isNull(4) ? OptionalQDate() : databaseModel()->convertFromServer(query.value(4).toDate());
            entityData_.remark    = query.isNull(5) ? OptionalQString() : query.value(5).toString();              
            createUserName_          = query.value(6).toString(); 
            updateUserName_          = query.value(7).toString(); 
            entityData_.createTime   = databaseModel()->convertFromServer(query.value(8).toDateTime()); 
            entityData_.updateTime   = databaseModel()->convertFromServer(query.value(9).toDateTime());
            entityData_.createUserId = query.value(10).toLongLong();
            entityData_.updateUserId = query.value(11).toLongLong();
            bankCode_ = query.value(12).toInt();
            ccdTableData_ = readCcdData(ccdQuery);
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

void IcdocCardPage::populateWidgets()
{
  if (entityId())
  { 
    // Contents page
    ui_.icdocDateEdit->setDate(entityData_.icdocDate);      
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
    populateCcdTable(ccdTableData_);
    nonPersistCcdTableData_ = ccdTableData_;
    // General data page
    ui_.createUserEdit->setText(createUserName_);
    ui_.createTimeEdit->setText(entityData_.createTime.toString(Qt::DefaultLocaleShortDate));
    ui_.updateUserEdit->setText(updateUserName_);
    ui_.updateTimeEdit->setText(entityData_.updateTime.toString(Qt::DefaultLocaleShortDate));

    contentsChanged_ = false;
  }
}    

bool IcdocCardPage::updateDatabase()
{
  static const char* querySql = "update \"ICDOC\" set " \
    "\"ICDOC_DATE\" = ?, \"BANK_ID\" = ?, \"STATE\" = ?, \"RECV_DATE\" = ?, " \
    "\"REMARK\" = ?, " \
    "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\' " \
    "where \"ICDOC_ID\" = ?";

  static const char* removeCcdQuerySql = "delete from \"CCD_ICDOC_RELATION\" where \"ICDOC_ID\" = ?";

  static const char* addCcdQuerySql = "insert into " \
    "\"CCD_ICDOC_RELATION\"(\"ICDOC_ID\", \"CCD_ID\", \"TAB_ORDER\") " \
    "values(?, ?, ?)";

  if (entityId())
  {        
    QSqlDatabase database(databaseModel()->database());
    if (database.transaction())
    {          
      QSqlQuery query(database);
      QSqlQuery removeCcdQuery(database);
      QSqlQuery insertCcdQuery(database);
      if (query.prepare(querySql) && removeCcdQuery.prepare(removeCcdQuerySql) 
        && insertCcdQuery.prepare(addCcdQuerySql))
      {
        query.bindValue(0, entityData_.icdocDate);
        query.bindValue(1, entityData_.bankId);
        query.bindValue(2, entityData_.state);
        query.bindValue(3, entityData_.recvDate ? entityData_.recvDate.get() : QVariant(QVariant::Date));          
        query.bindValue(4, entityData_.remark ? entityData_.remark.get() : QVariant(QVariant::String));          
        query.bindValue(5, databaseModel()->userId());
        query.bindValue(6, entityData_.icdocId);
        if (query.exec())
        {
          if (0 < query.numRowsAffected())
          {
            removeCcdQuery.bindValue(0, entityData_.icdocId);
            if (removeCcdQuery.exec())
            {                  
              if (insertCcdData(insertCcdQuery, entityData_.icdocId, ccdTableData_))
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

bool IcdocCardPage::insertIntoDatabase()
{
  static const char* querySql = "insert into " \
    "\"ICDOC\"(\"ICDOC_ID\", \"ICDOC_DATE\", \"BANK_ID\", \"STATE\", \"RECV_DATE\", " \
    "\"REMARK\", \"CREATE_USER_ID\", \"CREATE_TIME\", \"UPDATE_USER_ID\", \"UPDATE_TIME\") " \
    "values(?, ?, ?, ?, ?, ?, ?, 'NOW', ?, 'NOW')";
      
  static const char* addCcdQuerySql = "insert into " \
    "\"CCD_ICDOC_RELATION\"(\"ICDOC_ID\", \"CCD_ID\", \"TAB_ORDER\") " \
    "values(?, ?, ?)";

  if (OptionalQInt64 newEntityId = databaseModel()->generateId(icdocEntity))
  {      
    entityData_.icdocId = newEntityId.get();
    QSqlDatabase database(databaseModel()->database());
    if (database.transaction())
    {          
      QSqlQuery query(database);          
      QSqlQuery insertCcdQuery(database);
      if (query.prepare(querySql) && insertCcdQuery.prepare(addCcdQuerySql))
      {
        query.bindValue(0, entityData_.icdocId);            
        query.bindValue(1, entityData_.icdocDate);
        query.bindValue(2, entityData_.bankId);
        query.bindValue(3, entityData_.state);
        query.bindValue(4, entityData_.recvDate ? entityData_.recvDate.get() : QVariant(QVariant::Date));          
        query.bindValue(5, entityData_.remark ? entityData_.remark.get() : QVariant(QVariant::String));          
        query.bindValue(6, databaseModel()->userId());
        query.bindValue(7, databaseModel()->userId());
        if (query.exec())
        {
          if (0 < query.numRowsAffected())
          {                
            if (insertCcdData(insertCcdQuery, entityData_.icdocId, ccdTableData_))
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

bool IcdocCardPage::removeFromDatabase()
{
  static const char* querySql = "delete from \"ICDOC\" where \"ICDOC_ID\" = ?";
  static const char* ccdQuerySql = "delete from \"CCD_ICDOC_RELATION\" where \"ICDOC_ID\" = ?";

  if (QMessageBox::Yes == QMessageBox::question(this, tr("Inquiries on confirming documents"), 
    tr("Delete inquiry on confirming documents?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
  {
    if (entityId())
    {
      QSqlDatabase database(databaseModel()->database());
      if (database.transaction())
      {          
        QSqlQuery query(database);   
        QSqlQuery ccdQuery(database);               
        if (query.prepare(querySql) && ccdQuery.prepare(ccdQuerySql))
        {              
          ccdQuery.bindValue(0, entityData_.icdocId);
          if (ccdQuery.exec())
          {
            query.bindValue(0, entityData_.icdocId);
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

void IcdocCardPage::on_lineEdit_textEdited(const QString&)
{
  contentsChanged_ = true;
  updateWidgets();
}

void IcdocCardPage::on_textEdit_textChanged()
{
  contentsChanged_ = true;
  updateWidgets();
}

void IcdocCardPage::on_dateEdit_dateChanged(const QDate&)
{
  contentsChanged_ = true;
  updateWidgets();
}    

void IcdocCardPage::on_bankBtn_clicked(bool /*checked*/)
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
    std::auto_ptr<ListPage> listPage(new BankListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), 
      UserDefinedFilter(), UserDefinedOrder(), 0));
    ListSelectDialog listSelectDialog(this, listPage);
    listSelectDialog.setWindowModality(Qt::WindowModal);
    if (QDialog::Accepted == listSelectDialog.exec())
    {          
      selectedBankId_ = listSelectDialog.listPage()->selectedId();          
      if (OptionalQString bankShortName = listSelectDialog.listPage()->selectedRelationalText())
      {
        ui_.bankEdit->setText(bankShortName.get());
      }          
      contentsChanged_ = true;
      updateWidgets();
    }
  } 
}

void IcdocCardPage::on_addCcdBtn_clicked(bool /*checked*/)
{
  if (viewMode != mode())
  {        
    std::auto_ptr<ListPage> listPage(new CcdListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), 
      OptionalInt(), UserDefinedFilter(), UserDefinedOrder(), 0));
    ListSelectDialog listSelectDialog(this, listPage);
    listSelectDialog.setWindowModality(Qt::WindowModal);
    if (QDialog::Accepted == listSelectDialog.exec())
    {          
      if (OptionalQInt64 selectedPaysheetId = listSelectDialog.listPage()->selectedId())
      {
        if (addCcd(selectedPaysheetId.get()))
        {
          contentsChanged_ = true;
          updateWidgets();
        }
      }          
    }
  } 
}

void IcdocCardPage::on_removeCcdBtn_clicked(bool /*checked*/)
{
  if (viewMode != mode())
  {        
    int row = ui_.ccdTable->currentRow();
    if (row >= 0)
    {
      nonPersistCcdTableData_.erase(nonPersistCcdTableData_.begin() + row);
      populateCcdTable(nonPersistCcdTableData_);
      contentsChanged_ = true;
      updateWidgets();
    }
  }   
}

void IcdocCardPage::on_moveUpBtn_clicked(bool /*checked*/)
{
  if (viewMode != mode())
  {        
    int row = ui_.ccdTable->currentRow();
    if (0 < row)
    {
      int previousRow = row - 1;
      int columnCount = ui_.ccdTable->columnCount();
      for (int column = 0; column != columnCount; ++column)
      {
        std::auto_ptr<QTableWidgetItem> previousItem(ui_.ccdTable->takeItem(previousRow, column));
        std::auto_ptr<QTableWidgetItem> item(ui_.ccdTable->takeItem(row, column));
        ui_.ccdTable->setItem(previousRow, column, item.release());
        ui_.ccdTable->setItem(row, column, previousItem.release());
      }

      CcdData ccdData = nonPersistCcdTableData_[previousRow];
      nonPersistCcdTableData_[previousRow] = nonPersistCcdTableData_[row];
      nonPersistCcdTableData_[row] = ccdData;

      ui_.ccdTable->setCurrentCell(previousRow, ui_.ccdTable->currentColumn());
      contentsChanged_ = true;
      updateWidgets();
    }        
  }   
}

void IcdocCardPage::on_moveDownBtn_clicked(bool /*checked*/)
{
  if (viewMode != mode())
  {        
    int row = ui_.ccdTable->currentRow();
    if (0 <= row)
    {
      int rowCount = ui_.ccdTable->rowCount();
      if (row < rowCount - 1)
      {
        int nextRow = row + 1;
        int columnCount = ui_.ccdTable->columnCount();
        for (int column = 0; column != columnCount; ++column)
        {
          std::auto_ptr<QTableWidgetItem> nextItem(ui_.ccdTable->takeItem(nextRow, column));
          std::auto_ptr<QTableWidgetItem> item(ui_.ccdTable->takeItem(row, column));
          ui_.ccdTable->setItem(nextRow, column, item.release());
          ui_.ccdTable->setItem(row, column, nextItem.release());
        }

        CcdData ccdData = nonPersistCcdTableData_[nextRow];
        nonPersistCcdTableData_[nextRow] = nonPersistCcdTableData_[row];
        nonPersistCcdTableData_[row] = ccdData;

        ui_.ccdTable->setCurrentCell(nextRow, ui_.ccdTable->currentColumn());
        contentsChanged_ = true;
        updateWidgets();
      }          
    }        
  }   
}

bool IcdocCardPage::addCcd(qint64 ccdId)
{
  int rowCount = ui_.ccdTable->rowCount();
  if (rowCount < maxPayheets)
  {        
    if (!contains(nonPersistCcdTableData_, ccdId))
    {
      if (boost::optional<CcdData> ccdData = readCcdData(ccdId))
      {
        nonPersistCcdTableData_.push_back(*ccdData);
        populateCcdTable(nonPersistCcdTableData_);
        return true;
      }                    
    }        
  }
  return false;
}

void IcdocCardPage::on_ccdTable_currentCellChanged(int /*currentRow*/, int /*currentColumn*/, int /*previousRow*/, int /*previousColumn*/)
{
  updateCcdTableWidgets();
}

void IcdocCardPage::on_ccdTable_cellDoubleClicked(int row, int /*column*/)
{
  if (row >= 0 && row < nonPersistCcdTableData_.size())
  {
    showCcdCardTaskWindow(nonPersistCcdTableData_[row].ccd.ccdId);
  }      
}

void IcdocCardPage::on_ccdTable_headerContextMenu_requested(const QPoint& pos)
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

void IcdocCardPage::on_stateCheck_stateChanged(int /*state*/)
{
  contentsChanged_ = true;
  updateWidgets();
}

IcdocCardPage::CcdDataVector IcdocCardPage::readCcdData(QSqlQuery& query) const
{
  CcdDataVector vector;
  while (query.next())
  {
    CcdData data;
    data.ccd.ccdId  = query.value(0).toLongLong();
    data.ccd.number = query.value(1).toString();
    data.ccd.contractId = query.value(2).toLongLong();
    data.ccd.deppointId = query.value(3).toLongLong();
    data.ccd.exportPermDate = databaseModel()->convertFromServer(query.value(4).toDate());
    data.ccd.exportDate  = query.value(5).isNull() ? OptionalQDate() : databaseModel()->convertFromServer(query.value(5).toDate());
    data.ccd.productCode = query.value(6).isNull() ? OptionalQString() : query.value(6).toString();
    data.ccd.series      = query.value(7).isNull() ? OptionalQString() : query.value(7).toString();
    data.ccd.ttermId = query.value(8).toLongLong();
    data.ccd.amount  = query.value(9).toLongLong();
    data.ccd.duty    = query.value(10).isNull() ? OptionalQInt64() : query.value(10).toLongLong();
    data.ccd.rate    = query.value(11).isNull() ? OptionalQInt64() : query.value(11).toLongLong();
    data.ccd.zeroTaxPeriod   = query.value(12).toInt();
    data.ccd.paymentTermType = query.value(13).toInt();
    data.ccd.paymentTermDate = query.value(14).isNull() ? OptionalQDate() : databaseModel()->convertFromServer(query.value(14).toDate());
    data.ccd.paymentTermPeriod = query.value(15).isNull() ? OptionalInt() : query.value(15).toInt();
    data.paymentLimitDate = query.value(16).isNull() ? OptionalQDate() : databaseModel()->convertFromServer(query.value(16).toDate());
    data.zeroTaxLimitDate = databaseModel()->convertFromServer(query.value(17).toDate());
    data.ccd.createUserId = query.value(18).toLongLong();
    data.ccd.updateUserId = query.value(19).toLongLong();
    data.ccd.createTime   = databaseModel()->convertFromServer(query.value(20).toDateTime());
    data.ccd.updateTime   = databaseModel()->convertFromServer(query.value(21).toDateTime());
    data.contractNumber   = query.value(22).toString();
    data.passportNumber   = query.value(23).isNull() ? OptionalQString() : query.value(23).toString();
    data.currencyId       = query.value(24).toLongLong();
    data.currencyAlphabeticCode = query.value(25).toString();
    vector.append(data);
  }      
  return vector;
}

bool IcdocCardPage::insertCcdData(QSqlQuery& query, qint64 icdocId, const CcdDataVector& ccdData) const
{
  typedef CcdDataVector::const_iterator iterator;      
  int tabOrder = 0;
  query.bindValue(0, icdocId);
  for (iterator i = ccdData.begin(), end = ccdData.end(); i != end; ++i)
  {
    query.bindValue(1, i->ccd.ccdId);
    query.bindValue(2, tabOrder);
    if (!query.exec())
    {
      return false;
    }
    ++tabOrder;                    
  }
  return true;
}

bool IcdocCardPage::contains(const CcdDataVector& ccdData, qint64 ccdId)
{
  typedef CcdDataVector::const_iterator iterator;      
  for (iterator i = ccdData.begin(), end = ccdData.end(); i != end; ++i)
  {
    if (i->ccd.ccdId == ccdId)
    {
      return true;
    }
  }
  return false;
}

bool IcdocCardPage::contains(const CcdDataVector& ccdData, const ma::chrono::model::Ccd& ccd)
{
  return contains(ccdData, ccd.ccdId);
}

bool IcdocCardPage::contains(const CcdDataVector& ccdData, const ma::chrono::model::Currency& currency)
{
  typedef CcdDataVector::const_iterator iterator;      
  for (iterator i = ccdData.begin(), end = ccdData.end(); i != end; ++i)
  {
    if (i->currencyId == currency.currencyId)
    {
      return true;
    }
  }
  return false;
}

bool IcdocCardPage::contains(const CcdDataVector& ccdData, const ma::chrono::model::Contract& contract)
{
  typedef CcdDataVector::const_iterator iterator;      
  for (iterator i = ccdData.begin(), end = ccdData.end(); i != end; ++i)
  {
    if (i->ccd.contractId == contract.contractId)
    {
      return true;
    }
  }
  return false;
}
    
void IcdocCardPage::update(CcdDataVector& ccdData, const CcdData& updatedCcdData)
{
  typedef CcdDataVector::iterator iterator;      
  for (iterator i = ccdData.begin(), end = ccdData.end(); i != end; ++i)
  {
    if (i->ccd.ccdId == updatedCcdData.ccd.ccdId)
    {
      *i = updatedCcdData;
    }
  }      
}

void IcdocCardPage::update(CcdDataVector& ccdData, const ma::chrono::model::Currency& currency)
{
  typedef CcdDataVector::iterator iterator;      
  for (iterator i = ccdData.begin(), end = ccdData.end(); i != end; ++i)
  {
    if (i->currencyId == currency.currencyId)
    {
      i->currencyAlphabeticCode = currency.alphabeticCode;          
    }
  }              
}

void IcdocCardPage::update(CcdDataVector& ccdData, const ma::chrono::model::Contract& contract)
{
  typedef CcdDataVector::iterator iterator;      
  for (iterator i = ccdData.begin(), end = ccdData.end(); i != end; ++i)
  {
    if (i->ccd.contractId == contract.contractId)
    {
      i->contractNumber = contract.number;          
      i->passportNumber = contract.passportNumber;
    }
  }              
}   

void IcdocCardPage::remove(CcdDataVector& ccdData, qint64 ccdId)
{
  typedef CcdDataVector::iterator iterator;      
  iterator i = ccdData.begin();
  iterator end = ccdData.end();
  while (i != end)      
  {
    if (i->ccd.ccdId == ccdId)
    {
      i = ccdData.erase(i);
      end = ccdData.end();
    }
    else
    {
      ++i;
    }        
  }
}

boost::optional<IcdocCardPage::CcdData> IcdocCardPage::readCcdData(qint64 ccdId) const
{
  static const char* querySql = "select " \
    "t.\"NUMBER\", t.\"CONTRACT_ID\", t.\"DEPPOINT_ID\", t.\"EXPORT_PERM_DATE\", " \
    "t.\"EXPORT_DATE\", t.\"PRODUCT_CODE\", t.\"SERIES\", t.\"TTERM_ID\", t.\"AMOUNT\", " \
    "t.\"DUTY\", t.\"RATE\", t.\"ZERO_TAX_PERIOD\", t.\"PAYMENT_TERM_TYPE\", " \
    "t.\"PAYMENT_TERM_DATE\", t.\"PAYMENT_TERM_PERIOD\", t.\"PAYMENT_LIMIT_DATE\", t.\"ZERO_TAX_LIMIT_DATE\", " \
    "t.\"CREATE_USER_ID\", t.\"UPDATE_USER_ID\", t.\"CREATE_TIME\", t.\"UPDATE_TIME\", " \
    "c.\"NUMBER\", c.\"PASSPORT_NUMBER\", c.\"CURRENCY_ID\", cur.\"ALPHABETIC_CODE\" " \
    "from \"CCD_SEL\" t " \
    "join \"CONTRACT\" c on t.\"CONTRACT_ID\" = c.\"CONTRACT_ID\" " \
    "join \"CURRENCY\" cur on c.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
    "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
    "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
    "where t.\"CCD_ID\" = ?";

  QSqlDatabase database(databaseModel()->database());        
  QSqlQuery query(database);      
  if (query.prepare(querySql))
  {
    query.bindValue(0, ccdId);        
    if (query.exec())
    {
      if (query.next())
      {          
        CcdData data;
        data.ccd.ccdId  = ccdId;
        data.ccd.number = query.value(0).toString();
        data.ccd.contractId = query.value(1).toLongLong();
        data.ccd.deppointId = query.value(2).toLongLong();
        data.ccd.exportPermDate = databaseModel()->convertFromServer(query.value(3).toDate());
        data.ccd.exportDate  = query.value(4).isNull() ? OptionalQDate() : databaseModel()->convertFromServer(query.value(4).toDate());
        data.ccd.productCode = query.value(5).isNull() ? OptionalQString() : query.value(5).toString();
        data.ccd.series      = query.value(6).isNull() ? OptionalQString() : query.value(6).toString();
        data.ccd.ttermId = query.value(7).toLongLong();
        data.ccd.amount  = query.value(8).toLongLong();
        data.ccd.duty    = query.value(9).isNull() ? OptionalQInt64() : query.value(9).toLongLong();
        data.ccd.rate    = query.value(10).isNull() ? OptionalQInt64() : query.value(10).toLongLong();
        data.ccd.zeroTaxPeriod   = query.value(11).toInt();
        data.ccd.paymentTermType = query.value(12).toInt();
        data.ccd.paymentTermDate = query.value(13).isNull() ? OptionalQDate() : databaseModel()->convertFromServer(query.value(13).toDate());
        data.ccd.paymentTermPeriod = query.value(14).isNull() ? OptionalInt() : query.value(14).toInt();
        data.paymentLimitDate = query.value(15).isNull() ? OptionalQDate() : databaseModel()->convertFromServer(query.value(15).toDate());
        data.zeroTaxLimitDate = databaseModel()->convertFromServer(query.value(16).toDate());
        data.ccd.createUserId = query.value(17).toLongLong();
        data.ccd.updateUserId = query.value(18).toLongLong();
        data.ccd.createTime   = databaseModel()->convertFromServer(query.value(19).toDateTime());
        data.ccd.updateTime   = databaseModel()->convertFromServer(query.value(20).toDateTime());
        data.contractNumber   = query.value(21).toString();
        data.passportNumber   = query.value(22).isNull() ? OptionalQString() : query.value(22).toString();
        data.currencyId       = query.value(23).toLongLong();
        data.currencyAlphabeticCode = query.value(24).toString();
        return data;
      }
    }
  }      
  return boost::optional<CcdData>();
}    

void IcdocCardPage::populateCcdTable(const CcdDataVector& vector)
{      
  typedef CcdDataVector::const_iterator iterator;      
  static const QVariant leftAlignment = QVariant(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
  static const QVariant rightAlignment = QVariant(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
  int row = 0;      
  ui_.ccdTable->setRowCount(vector.size());
  for (iterator i = vector.begin(), end = vector.end(); i != end; ++i)
  {                        
    std::auto_ptr<QTableWidgetItem> item(new QTableWidgetItem(i->ccd.number));
    item->setData(Qt::TextAlignmentRole, rightAlignment);                
    ui_.ccdTable->setItem(row, 0, item.release());

    item.reset(new QTableWidgetItem(model::CurrencyAmount(i->ccd.amount).toString()));
    item->setData(Qt::TextAlignmentRole, rightAlignment);                
    ui_.ccdTable->setItem(row, 1, item.release());

    item.reset(new QTableWidgetItem(i->currencyAlphabeticCode));
    item->setData(Qt::TextAlignmentRole, leftAlignment);                
    ui_.ccdTable->setItem(row, 2, item.release());

    item.reset(new QTableWidgetItem(i->contractNumber));
    item->setData(Qt::TextAlignmentRole, rightAlignment);                
    ui_.ccdTable->setItem(row, 3, item.release());

    item.reset(new QTableWidgetItem(i->passportNumber ? i->passportNumber.get() : QString()));
    item->setData(Qt::TextAlignmentRole, rightAlignment);                
    ui_.ccdTable->setItem(row, 4, item.release());
        
    ++row;        
  }      
}    

void IcdocCardPage::showRequiredMarkers(bool visible)
{
  WidgetUtility::show(requiredMarkers_, visible);
}

void IcdocCardPage::updateCcdTableWidgets()
{
  if (viewMode == mode()) 
  {
    ui_.addCcdBtn->setEnabled(false);
    ui_.removeCcdBtn->setEnabled(false);
    ui_.moveUpBtn->setEnabled(false);
    ui_.moveDownBtn->setEnabled(false);
  }
  else
  {
    int currentRow = ui_.ccdTable->currentRow();
    int rowCount = ui_.ccdTable->rowCount();
    ui_.addCcdBtn->setEnabled(rowCount < maxPayheets);
    ui_.removeCcdBtn->setEnabled(rowCount > 0 && currentRow >= 0);
    ui_.moveUpBtn->setEnabled(currentRow > 0);
    ui_.moveDownBtn->setEnabled(currentRow >= 0 && currentRow < rowCount - 1);
  }      
}

void IcdocCardPage::on_createUserBtn_clicked(bool /*checked*/)
{
  if (entityId())
  {      
    showUserCardTaskWindow(entityData_.createUserId);
  }        
}

void IcdocCardPage::on_updateUserBtn_clicked(bool /*checked*/)
{
  if (entityId())
  {      
    showUserCardTaskWindow(entityData_.updateUserId);
  }        
}

void IcdocCardPage::on_database_userUpdated(const ma::chrono::model::User& user)
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

void IcdocCardPage::on_database_bankUpdated(const ma::chrono::model::Bank& bank)
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

void IcdocCardPage::on_database_bankRemoved(const ma::chrono::model::Bank& bank)
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

void IcdocCardPage::on_database_ccdUpdated(const ma::chrono::model::Ccd& ccd)
{
  boost::optional<CcdData> updatedCcdData;
  if (contains(ccdTableData_, ccd))
  {
    updatedCcdData = readCcdData(ccd.ccdId);
    if (updatedCcdData)
    {
      update(ccdTableData_, *updatedCcdData);
    }
  }
  if (contains(nonPersistCcdTableData_, ccd))
  {
    if (!updatedCcdData)
    {
      updatedCcdData = readCcdData(ccd.ccdId);
    }
    if (updatedCcdData)
    {
      update(nonPersistCcdTableData_, *updatedCcdData);
      populateCcdTable(nonPersistCcdTableData_);
      updateWidgets();
    }
  }      
}

void IcdocCardPage::on_database_ccdRemoved(const ma::chrono::model::Ccd& ccd)
{
  boost::optional<CcdData> updatedCcdData;
  if (contains(ccdTableData_, ccd))
  {
    remove(ccdTableData_, ccd.ccdId);        
  }
  if (contains(nonPersistCcdTableData_, ccd))
  {
    remove(nonPersistCcdTableData_, ccd.ccdId);
    populateCcdTable(nonPersistCcdTableData_);
    updateWidgets();
  }      
}

void IcdocCardPage::on_database_currencyUpdated(const ma::chrono::model::Currency& currency)
{      
  if (contains(ccdTableData_, currency))
  {        
    update(ccdTableData_, currency);        
  }
  if (contains(nonPersistCcdTableData_, currency))
  {
    update(nonPersistCcdTableData_, currency);
    populateCcdTable(nonPersistCcdTableData_);
    updateWidgets();
  }
}

void IcdocCardPage::on_database_contractUpdated(const ma::chrono::model::Contract& contract)
{      
  if (contains(ccdTableData_, contract))
  {        
    update(ccdTableData_, contract);
  }
  if (contains(nonPersistCcdTableData_, contract))
  {
    update(nonPersistCcdTableData_, contract);
    populateCcdTable(nonPersistCcdTableData_);
    updateWidgets();
  }
}   

static QString findTemplate(int bankCode)
{
  return QCoreApplication::applicationDirPath() + QString::fromWCharArray(L"/icdoc/%1.xlt").arg(bankCode);
}

static bool checkTemplateExistance(const QString& filename)
{
  return QFile::exists(filename);
}

void IcdocCardPage::on_sendToExcelAction_triggered()
{
  QString templateFilename = findTemplate(bankCode_);
  if (!checkTemplateExistance(templateFilename))
  {
    //todo
    return;
  }
  ExcelExporter::exportIcdoc(entityData_.icdocId, templateFilename, *databaseModel());
  //todo
}

} // namespace chrono
} //namespace ma

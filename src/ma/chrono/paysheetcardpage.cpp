/*
TRANSLATOR ma::chrono::PaysheetCardPage
*/

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <QAction>
#include <QMessageBox>
#include <QCalendarWidget>
#include <QSqlQuery>
#include <QSqlError>
#include <ma/chrono/constants.h>
#include <ma/chrono/dbconstants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/listselectdialog.h>
#include <ma/chrono/contractlistpage.h>
#include <ma/chrono/ccdlistpage.h>
#include <ma/chrono/currencylistpage.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/widgetutility.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/ccd.h>
#include <ma/chrono/model/currency.h>
#include <ma/chrono/model/currencyamount.h>
#include <ma/chrono/localeutility.h>
#include <ma/chrono/paysheetcardpage.h>

namespace ma
{
  namespace chrono
  {    
    PaysheetCardPage::PaysheetCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent)
      : CardPage(resourceManager, taskWindowManager, databaseModel, helpAction, paysheetEntity, mode, entityId, parent)
      , dataAwareWidgets_()
      , contentsChanged_(false)     
    {
      ui_.setupUi(this);          
      ui_.tabWidget->setCurrentIndex(0);
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      LocaleUtility::setupLocaleAwareWidget(ui_.payDate);
      createAction()->setText(tr("&New pay sheet"));
      dataAwareWidgets_ << ui_.amountEdit << ui_.payDate << ui_.docNumberEdit
                        << ui_.accountNumberEdit << ui_.remarkTextEdit;
      requiredMarkers_  << ui_.asterikLabel << ui_.asterikLabel_2 << ui_.asterikLabel_3 
                        << ui_.asterikLabel_4 << ui_.asterikLabel_5;
      ui_.removeCcdBtn->setIcon(resourceManager->getIcon(clearEditIconName));
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
      QObject::connect(databaseModel.get(), SIGNAL(userUpdated(const ma::chrono::model::User&)), SLOT(on_database_userUpdated(const ma::chrono::model::User&)));
      QObject::connect(databaseModel.get(), SIGNAL(contractUpdated(const ma::chrono::model::Contract&)), SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));
      QObject::connect(databaseModel.get(), SIGNAL(contractRemoved(const ma::chrono::model::Contract&)), SLOT(on_database_contractRemoved(const ma::chrono::model::Contract&)));      
      QObject::connect(databaseModel.get(), SIGNAL(ccdUpdated(const ma::chrono::model::Ccd&)), SLOT(on_database_ccdUpdated(const ma::chrono::model::Ccd&)));
      QObject::connect(databaseModel.get(), SIGNAL(ccdRemoved(const ma::chrono::model::Ccd&)), SLOT(on_database_ccdRemoved(const ma::chrono::model::Ccd&)));      
      QObject::connect(databaseModel.get(), SIGNAL(currencyUpdated(const ma::chrono::model::Currency&)), SLOT(on_database_currencyUpdated(const ma::chrono::model::Currency&)));
      QObject::connect(databaseModel.get(), SIGNAL(currencyRemoved(const ma::chrono::model::Currency&)), SLOT(on_database_currencyRemoved(const ma::chrono::model::Currency&)));      
      updateWidgets();      
    }

    PaysheetCardPage::~PaysheetCardPage()
    {
    } 

    void PaysheetCardPage::refresh()
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

    void PaysheetCardPage::save()
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

    void PaysheetCardPage::cancel()
    {
      if (editMode == mode())
      {      
        populateWidgets();
      }      
      CardPage::cancel();
      updateWidgets();
    }

    void PaysheetCardPage::edit()
    {      
      CardPage::edit();
      updateWidgets();
    }

    void PaysheetCardPage::remove()
    {
      if (createMode != mode())
      {
        if (removeFromDatabase())
        {
          CardPage::remove();
        }
      }            
    }

    std::auto_ptr<CardPage> PaysheetCardPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new PaysheetCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }

    void PaysheetCardPage::setSelectedContractId(qint64 contractId)
    {
      selectedContractId_ = contractId;            
      updateContractRelatedNonPersistData(contractId);
    }

    void PaysheetCardPage::connectDataAwareWidgets()
    {
      
      typedef QWidgetList::const_iterator const_iterator;
      
      for (const_iterator it = dataAwareWidgets_.begin(), end = dataAwareWidgets_.end(); it != end; ++it)
      {
        QWidget* widget = *it;        
        if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget))
        {
          QObject::connect(lineEdit, SIGNAL(textEdited(const QString&)), SLOT(on_lineEdit_textEdited(const QString&)));
        }
        else if (QTextEdit* textEdit = qobject_cast<QTextEdit*>(widget))
        {
          QObject::connect(textEdit, SIGNAL(textChanged()), SLOT(on_textEdit_textChanged()));
        }        
        else if (QDateEdit* dateEdit = qobject_cast<QDateEdit*>(widget))
        {
          QObject::connect(dateEdit, SIGNAL(dateChanged(const QDate&)), SLOT(on_dateEdit_dateChanged(const QDate&)));
        }
      }
    }

    void PaysheetCardPage::updateWidgets()
    {      
      
      typedef QWidgetList::const_iterator const_iterator;

      Mode currentMode = mode();
      bool readOnly = viewMode == currentMode;
      WidgetUtility::setReadOnly(dataAwareWidgets_, readOnly);

      bool peristance = createMode != currentMode && entityId();      
      int generalTabIndex = ui_.tabWidget->indexOf(ui_.generalTab);      
      ui_.tabWidget->setTabEnabled(generalTabIndex, peristance);            
      ui_.ccdBtn->setEnabled(viewMode != currentMode || selectedCcdId_);
      ui_.removeCcdBtn->setVisible(viewMode != currentMode && selectedCcdId_);
      switch (currentMode)
      {
      case viewMode:
        if (entityId())
        {          
          updateWindowTitle(tr("Pay sheets - %2 by contract %1 by %3")
            .arg(contractNumber_).arg(entityData_.docNumber).arg(entityData_.payDate.toString(Qt::DefaultLocaleShortDate)));
        }
        showRequiredMarkers(false);
        break;

      case editMode:
        if (entityId())
        {
          if (contentsChanged_)
          {
            updateWindowTitle(tr("Pay sheets - %2 by contract %1 by %3*")
            .arg(contractNumber_).arg(entityData_.docNumber).arg(entityData_.payDate.toString(Qt::DefaultLocaleShortDate)));
          }
          else
          {
            updateWindowTitle(tr("Pay sheets - %2 by contract %1 by %3")
            .arg(contractNumber_).arg(entityData_.docNumber).arg(entityData_.payDate.toString(Qt::DefaultLocaleShortDate)));
          }        
        }
        showRequiredMarkers(true);
        break;

      case createMode:
        updateWindowTitle(tr("Pay sheets - New pay sheet*"));
        showRequiredMarkers(true);
        break;
      }
    }

    bool PaysheetCardPage::populateFromWidgets()
    {
      static const char* docNumberTitle = QT_TR_NOOP("Document number");            
      static const char* contractTitle = QT_TR_NOOP("Contract");            
      static const char* amountTitle = QT_TR_NOOP("Amount");            
      static const char* currencyTitle = QT_TR_NOOP("Currency");            
      static const char* accountNumberTitle = QT_TR_NOOP("Account number");            
      static const char* remarkTitle = QT_TR_NOOP("Remark");      
      
      static const char* messageTitle = QT_TR_NOOP("Invalid data input");
      static const char* notEmpty = QT_TR_NOOP("Field \"%1\" can\'t be empty.");
      static const char* tooLongText = QT_TR_NOOP("Field \"%1\" can\'t exceed %2 characters.");      
      static const char* invalidFieldValueText = QT_TR_NOOP("Invalid value in field \"%1\".");      
      static const char* wrongCcdContract = QT_TR_NOOP("Selected CCD belongs to another contract than You selected. Select another CCD or contract.");      
            
      int contentsTabIndex = ui_.tabWidget->indexOf(ui_.contentsTab);
      bool validated;
      // Get document number
      QString documentNumber = ui_.docNumberEdit->text();            
      if (documentNumber.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(docNumberTitle)));
      } 
      else if (documentNumber.length() > model::Paysheet::docNumberMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(docNumberTitle)).arg(model::Paysheet::docNumberMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.docNumberEdit->setFocus(Qt::TabFocusReason);
        ui_.docNumberEdit->selectAll();
        return false;
      } 
      // Get contract
      if (!selectedContractId_)
      {        
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(contractTitle)));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.contractBtn->setFocus(Qt::TabFocusReason);
        return false;
      } 
      if (selectedCcdId_)
      {
        if (selectedContractId_ && selectedCcdContractId_)
        {
          if (selectedContractId_.get() != selectedCcdContractId_.get())
          {
            QMessageBox::warning(this, tr(messageTitle), tr(wrongCcdContract));
            ui_.tabWidget->setCurrentIndex(contentsTabIndex);
            ui_.ccdBtn->setFocus(Qt::TabFocusReason);
            return false;
          }
        }
      }
      // Get currency
      if (!selectedCurrencyId_)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(currencyTitle)));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.currencyBtn->setFocus(Qt::TabFocusReason);
        return false;
      }
      // Get amount
      model::CurrencyAmount currencyAmount;      
      if (!currencyAmount.setValue(ui_.amountEdit->text().trimmed()))
      {
        QMessageBox::warning(this, tr(messageTitle), tr(invalidFieldValueText).arg(tr(amountTitle)));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.amountEdit->setFocus(Qt::TabFocusReason);
        ui_.amountEdit->selectAll();
        return false;
      }
      // Get account number      
      QString accountNumber = ui_.accountNumberEdit->text();
      if (accountNumber.length() > model::Paysheet::accountNumberMaxLen)
      {
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(accountNumberTitle)).arg(model::Paysheet::accountNumberMaxLen));        
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.accountNumberEdit->setFocus(Qt::TabFocusReason);
        ui_.accountNumberEdit->selectAll();
        return false;
      }
      // Get remark      
      QString remark = ui_.remarkTextEdit->toPlainText().trimmed();
      if (remark.length() > model::Paysheet::remarkMaxLen)
      {
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(remarkTitle)).arg(model::Paysheet::remarkMaxLen));        
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.remarkTextEdit->setFocus(Qt::TabFocusReason);
        ui_.remarkTextEdit->selectAll();
        return false;
      }
                      
      entityData_.docNumber = documentNumber;
      entityData_.contractId = selectedContractId_.get();
      entityData_.ccdId = selectedCcdId_;
      entityData_.payDate = ui_.payDate->date();
      entityData_.amount = currencyAmount.amount();
      entityData_.currencyId = selectedCurrencyId_.get();
      entityData_.accountNumber = accountNumber.isEmpty() ? OptionalQString() : accountNumber;      
      entityData_.remark = remark.isEmpty() ? OptionalQString() : remark;
      return true;
    }

    bool PaysheetCardPage::populateFromDatabase()
    {      
      static const char* querySql = "select " \
        "t.\"CONTRACT_ID\", cr.\"NUMBER\", " \
        "t.\"CCD_ID\", c.\"NUMBER\", " \
        "t.\"DOC_NUMBER\", t.\"PAY_DATE\", t.\"AMOUNT\", " \
        "cur.\"FULL_NAME\", t.\"ACCOUNT_NUMBER\", t.\"REMARK\", " \
        "t.\"CREATE_USER_ID\", t.\"UPDATE_USER_ID\", " \
        "cu.\"LOGIN\", uu.\"LOGIN\", " \
        "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", t.\"CURRENCY_ID\" " \
        "from \"PAY_SHEET\" t " \
        "join \"CONTRACT\" cr on t.\"CONTRACT_ID\" = cr.\"CONTRACT_ID\" " \
        "left join \"CCD\" c on t.\"CCD_ID\" = c.\"CCD_ID\" " \
        "join \"CURRENCY\" cur on t.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
        "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
        "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
        "where t.\"PAY_SHEET_ID\" = ?";

      if (entityId())
      {      
        QSqlQuery query(databaseModel()->database());
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityId().get());
          if (query.exec())
          {
            if (query.next())
            { 
              entityData_.paysheetId = entityId().get();
              entityData_.contractId = query.value(0).toLongLong();
              contractNumber_        = query.value(1).toString();
              entityData_.ccdId      = query.value(2).isNull() ? OptionalQInt64() : query.value(2).toLongLong();
              ccdNumber_             = query.value(2).isNull() ? OptionalQString() : query.value(3).toString();
              entityData_.docNumber  = query.value(4).toString();
              entityData_.payDate    = databaseModel()->convertFromServer(query.value(5).toDate());
              entityData_.amount     = query.value(6).toLongLong();
              currencyFullName_      = query.value(7).toString();
              entityData_.accountNumber = query.value(8).isNull() ? OptionalQString() : query.value(8).toString();              
              entityData_.remark     = query.isNull(9) ? OptionalQString() : query.value(9).toString();
              entityData_.createUserId = query.value(10).toLongLong();
              entityData_.updateUserId = query.value(11).toLongLong();
              createUserName_          = query.value(12).toString(); 
              updateUserName_          = query.value(13).toString(); 
              entityData_.createTime   = databaseModel()->convertFromServer(query.value(14).toDateTime()); 
              entityData_.updateTime   = databaseModel()->convertFromServer(query.value(15).toDateTime());              
              entityData_.currencyId   = query.value(16).toLongLong();
              return true;
            }
          }
        }
      }
      return false;
    }

    void PaysheetCardPage::populateWidgets()
    {
      if (entityId())
      { 
        // Contents page
        ui_.docNumberEdit->setText(entityData_.docNumber);      
        ui_.contractEdit->setText(contractNumber_);
        selectedCcdContractId_ = selectedContractId_ = entityData_.contractId;        
        ui_.ccdEdit->setText(ccdNumber_ ? ccdNumber_.get() : QString());
        selectedCcdId_ = entityData_.ccdId;
        ui_.payDate->setDate(entityData_.payDate);
        ui_.amountEdit->setText(model::CurrencyAmount(entityData_.amount).toString());               
        ui_.currencyEdit->setText(currencyFullName_);
        selectedCurrencyId_ = entityData_.currencyId;
        ui_.remarkTextEdit->setPlainText(entityData_.remark ? entityData_.remark.get() : QString());
        // General data page
        ui_.createUserEdit->setText(createUserName_);
        ui_.createTimeEdit->setText(entityData_.createTime.toString(Qt::DefaultLocaleShortDate));
        ui_.updateUserEdit->setText(updateUserName_);
        ui_.updateTimeEdit->setText(entityData_.updateTime.toString(Qt::DefaultLocaleShortDate));
        populateExportData();
        contentsChanged_ = false;
      }
    } 

    void PaysheetCardPage::populateExportData()
    {
      static QString documentNumberCaption = tr("Document number");
      static QString contractNumberCaption = tr("Contract number");      
      static QString ccdNumberCaption = tr("CCD number");
      static QString payDateCaption = tr("Pay date");      
      static QString amountCaption = tr("Amount");      
      static QString currencyFullNameCaption = tr("Currency");      
      static QString accountNumberCaption = tr("Account number");      
      static QString remarksCaption = tr("Remarks");      
      static QString createUserCaption = tr("User, creator");
      static QString createTimeCaption = tr("Time, created");
      static QString updateUserCaption = tr("User, last update");
      static QString updateTimeCaption = tr("Time, last update");                    
        
      ExportDataList exportDataList;
      exportDataList << std::make_pair(documentNumberCaption, QVariant(entityData_.docNumber));
      exportDataList << std::make_pair(contractNumberCaption, QVariant(contractNumber_));
      exportDataList << std::make_pair(ccdNumberCaption, QVariant::fromValue(ccdNumber_));
      exportDataList << std::make_pair(payDateCaption, QVariant(entityData_.payDate));      
      exportDataList << std::make_pair(amountCaption, QVariant::fromValue(model::CurrencyAmount(entityData_.amount)));
      exportDataList << std::make_pair(currencyFullNameCaption, QVariant(currencyFullName_));
      exportDataList << std::make_pair(accountNumberCaption, QVariant::fromValue(entityData_.accountNumber));
      exportDataList << std::make_pair(remarksCaption, entityData_.remark ? QVariant(entityData_.remark.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(createUserCaption, QVariant(createUserName_));
      exportDataList << std::make_pair(createTimeCaption, QVariant(entityData_.createTime));
      exportDataList << std::make_pair(updateUserCaption, QVariant(updateUserName_));
      exportDataList << std::make_pair(updateTimeCaption, QVariant(entityData_.updateTime));      

      setExportDataList(exportDataList);
    }

    bool PaysheetCardPage::updateDatabase()
    {
      static const char* querySql = "update \"PAY_SHEET\" set " \
        "\"DOC_NUMBER\" = ?, \"ACCOUNT_NUMBER\" = ?, \"AMOUNT\" = ?, \"CONTRACT_ID\" = ?, " \
        "\"CCD_ID\" = ?, \"CURRENCY_ID\" = ?,  \"PAY_DATE\" = ?, " \
        "\"REMARK\" = ?, " \
        "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\' " \
        "where \"PAY_SHEET_ID\" = ?";

      if (entityId())
      {        
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {          
          query.bindValue(0, entityData_.docNumber);
          query.bindValue(1, entityData_.accountNumber ? entityData_.accountNumber.get() : QVariant(QVariant::String));
          query.bindValue(2, entityData_.amount);
          query.bindValue(3, entityData_.contractId);          
          query.bindValue(4, entityData_.ccdId ? entityData_.ccdId.get() : QVariant(QVariant::LongLong));
          query.bindValue(5, entityData_.currencyId);          
          query.bindValue(6, databaseModel()->convertToServer(entityData_.payDate));          
          query.bindValue(7, entityData_.remark ? entityData_.remark.get() : QVariant(QVariant::String));          
          query.bindValue(8, databaseModel()->userId());
          query.bindValue(9, entityData_.paysheetId);
          if (query.exec())
          {
            if (0 < query.numRowsAffected())
            {
              databaseModel()->notifyUpdate(entityData_);
              return true;
            }            
          }
        }
      }
      return false;
    }

    bool PaysheetCardPage::insertIntoDatabase()
    {      
      static const char* querySql = "insert into \"PAY_SHEET\" (" \
        "\"PAY_SHEET_ID\", \"DOC_NUMBER\", \"ACCOUNT_NUMBER\", \"AMOUNT\", " \
        "\"CURRENCY_ID\", \"CONTRACT_ID\", \"CCD_ID\", " \
        "\"REMARK\", \"PAY_DATE\", " \
        "\"CREATE_USER_ID\", \"UPDATE_USER_ID\", " \
        "\"CREATE_TIME\", \"UPDATE_TIME\") " \
        "values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, \'NOW\', \'NOW\')";            

      if (OptionalQInt64 newEntityId = databaseModel()->generateId(paysheetEntity))
      {
        entityData_.paysheetId = newEntityId.get();    
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.paysheetId);
          query.bindValue(1, entityData_.docNumber);
          query.bindValue(2, entityData_.accountNumber ? entityData_.accountNumber.get() : QVariant(QVariant::String));
          query.bindValue(3, entityData_.amount);
          query.bindValue(4, entityData_.currencyId);
          query.bindValue(5, entityData_.contractId);
          query.bindValue(6, entityData_.ccdId ? entityData_.ccdId.get() : QVariant(QVariant::LongLong));
          query.bindValue(7, entityData_.remark ? entityData_.remark.get() : QVariant(QVariant::String));      
          query.bindValue(8, databaseModel()->convertToServer(entityData_.payDate));                    
          query.bindValue(9, databaseModel()->userId());
          query.bindValue(10, databaseModel()->userId());      
          if (query.exec())
          {
            if (0 < query.numRowsAffected())
            {
              setEntityId(newEntityId.get());
              databaseModel()->notifyInsert(entityData_);
              return true;
            }            
          }
        }        
      }
      return false;
    }

    bool PaysheetCardPage::removeFromDatabase()
    {
      static const char* querySql = "delete from \"PAY_SHEET\" where \"PAY_SHEET_ID\" = ?";

      if (QMessageBox::Yes == QMessageBox::question(this, tr("Pay sheets"), 
        tr("Delete pay sheet?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
      {
        if (entityId())
        {
          QSqlQuery query(databaseModel()->database());              
          if (query.prepare(querySql))
          {
            query.bindValue(0, entityData_.paysheetId);
            if (query.exec())
            {
              if (0 < query.numRowsAffected())
              {
                databaseModel()->notifyRemove(entityData_);
                return true;
              }            
            }
          }
        }
      }
      return false;
    }

    void PaysheetCardPage::on_lineEdit_textEdited(const QString&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void PaysheetCardPage::on_textEdit_textChanged()
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void PaysheetCardPage::on_dateEdit_dateChanged(const QDate&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }    

    void PaysheetCardPage::on_contractBtn_clicked(bool /*checked*/)
    {
      if (viewMode == mode())
      {
        if (entityId())
        {          
          showContractCardTaskWindow(entityData_.contractId);
        }        
      }
      else
      {
        //todo: add default selection
        std::auto_ptr<ListPage> listPage(new ContractListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), 
          OptionalInt(), UserDefinedFilter(), UserDefinedOrder(), 0));
        ListSelectDialog listSelectDialog(this, listPage);
        listSelectDialog.setWindowModality(Qt::WindowModal);
        if (QDialog::Accepted == listSelectDialog.exec())
        {          
          selectedContractId_ = listSelectDialog.listPage()->selectedId();          
          if (OptionalQString contractNumber = listSelectDialog.listPage()->selectedRelationalText())
          {
            ui_.contractEdit->setText(contractNumber.get());
          }
          contentsChanged_ = true;
          updateWidgets();
        }
      } 
    }
    
    void PaysheetCardPage::on_ccdBtn_clicked(bool /*checked*/)
    {
      if (viewMode == mode())
      {
        if (entityId() && entityData_.ccdId)
        {          
          showCcdCardTaskWindow(entityData_.ccdId.get());
        }        
      }
      else
      {
        //todo: add default selection
        std::auto_ptr<ListPage> listPage(new CcdListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), 
          OptionalInt(), UserDefinedFilter(), UserDefinedOrder(), 0));
        ListSelectDialog listSelectDialog(this, listPage);
        listSelectDialog.setWindowModality(Qt::WindowModal);
        if (QDialog::Accepted == listSelectDialog.exec())
        {          
          selectedCcdId_ = listSelectDialog.listPage()->selectedId();          
          if (OptionalQString ccdNumber = listSelectDialog.listPage()->selectedRelationalText())
          {
            ui_.ccdEdit->setText(ccdNumber.get());
          }
          updateCcdRelatedNonPersistData(selectedCcdId_.get());
          contentsChanged_ = true;
          updateWidgets();
        }
      } 
    }

    void PaysheetCardPage::on_currencyBtn_clicked(bool /*checked*/)
    {
      if (viewMode == mode())
      {
        if (entityId())
        {          
          showCurrencyCardTaskWindow(entityData_.currencyId);
        }        
      }
      else
      {
        //todo: add default selection
        std::auto_ptr<ListPage> listPage(new CurrencyListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), 
          UserDefinedFilter(), UserDefinedOrder(), 0));
        ListSelectDialog listSelectDialog(this, listPage);
        listSelectDialog.setWindowModality(Qt::WindowModal);
        if (QDialog::Accepted == listSelectDialog.exec())
        {          
          selectedCurrencyId_ = listSelectDialog.listPage()->selectedId();          
          if (OptionalQString currencyNumber = listSelectDialog.listPage()->selectedRelationalText())
          {
            ui_.currencyEdit->setText(currencyNumber.get());
          }
          contentsChanged_ = true;
          updateWidgets();
        }
      } 
    } 

    void PaysheetCardPage::on_removeCcdBtn_clicked(bool /*checked*/)
    {
      if (viewMode != mode())
      {
        ui_.ccdEdit->setText(QString());
        selectedCcdId_.reset();
        selectedCcdContractId_.reset();
        updateWidgets();
      }
    }

    void PaysheetCardPage::showRequiredMarkers(bool visible)
    {
      typedef QList<QLabel*> list_type;
      typedef list_type::const_iterator iterator_type;
      for (iterator_type i = requiredMarkers_.begin(), e = requiredMarkers_.end(); i != e; ++i)
      {
        (*i)->setVisible(visible);
      }
    }

    void PaysheetCardPage::updateCcdRelatedNonPersistData(qint64 ccdId)
    {
      static const char* querySql = "select " \
        "t.\"CONTRACT_ID\", con.\"NUMBER\" " \
        "from \"CCD\" t " \
        "join \"CONTRACT\" con on con.\"CONTRACT_ID\" = t.\"CONTRACT_ID\" " \
        "where t.\"CCD_ID\" = ?";
            
      if (selectedCcdId_)
      {      
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {          
          query.bindValue(0, ccdId);
          if (query.exec())
          {
            if (query.next())
            { 
              selectedCcdContractId_ = selectedContractId_ = query.value(0).toLongLong();
              ui_.contractEdit->setText(query.value(1).toString());
            }
          }
        }
      }
    }

    void PaysheetCardPage::updateContractRelatedNonPersistData(qint64 contractId)
    {
      static const char* querySql = "select " \
        "con.\"NUMBER\", con.\"PASSPORT_NUMBER\", " \
        "con.\"BANK_ID\", b.\"SHORT_NAME\", " \
        "con.\"COUNTERPART_ID\", con.\"CURRENCY_ID\", " \
        "cnt.\"SHORT_NAME\", cur.\"FULL_NAME\", " \
        "con.\"END_DATE\" " \
        "from \"CONTRACT\" con " \
        "join \"BANK\" b on con.\"BANK_ID\" = b.\"BANK_ID\" " \
        "join \"COUNTERPART\" cnt on con.\"COUNTERPART_ID\" = cnt.\"COUNTERPART_ID\" " \
        "join \"CURRENCY\" cur on con.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
        "where con.\"CONTRACT_ID\" = ?";

      if (selectedContractId_)
      {      
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {          
          query.bindValue(0, contractId);
          if (query.exec())
          {
            if (query.next())
            {                            
              ui_.contractEdit->setText(query.value(0).toString());                          
            }
          }
        }
      }
    }

    void PaysheetCardPage::on_createUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.createUserId);
      }        
    }

    void PaysheetCardPage::on_updateUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.updateUserId);
      }        
    }

    void PaysheetCardPage::on_database_userUpdated(const ma::chrono::model::User& user)
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

    void PaysheetCardPage::on_database_contractUpdated(const ma::chrono::model::Contract& contract)
    {
      if (entityId())
      {        
        if (entityData_.contractId == contract.contractId)
        {
          contractNumber_ = contract.number;
        }
      }
      if (selectedContractId_)
      {
        if (selectedContractId_.get() == contract.contractId)
        {
          ui_.contractEdit->setText(contract.number);            
          updateWidgets();
        }            
      }      
    }

    void PaysheetCardPage::on_database_contractRemoved(const ma::chrono::model::Contract& contract)
    {
      if (selectedContractId_)
      {
        if (selectedContractId_.get() == contract.contractId)
        {
          ui_.contractEdit->setText(QString());
          selectedContractId_.reset();
          updateWidgets();
        }
      }
    }
    
    void PaysheetCardPage::on_database_ccdUpdated(const ma::chrono::model::Ccd& ccd)
    {
      if (entityId())
      {        
        if (entityData_.ccdId == ccd.ccdId)
        {
          ccdNumber_ = ccd.number;
        }
      }
      if (selectedCcdId_)
      {
        if (selectedCcdId_.get() == ccd.ccdId)
        {
          ui_.ccdEdit->setText(ccd.number);            
          updateWidgets();
        }            
      }      
    }

    void PaysheetCardPage::on_database_ccdRemoved(const ma::chrono::model::Ccd& ccd)
    {
      if (selectedCcdId_)
      {
        if (selectedCcdId_.get() == ccd.ccdId)
        {
          ui_.ccdEdit->setText(QString());
          selectedCcdId_.reset();
          selectedCcdContractId_.reset();
          updateWidgets();
        }
      }
    } 

    void PaysheetCardPage::on_database_currencyUpdated(const ma::chrono::model::Currency& currency)
    {
      if (entityId())
      {        
        if (entityData_.currencyId == currency.currencyId)
        {
          currencyFullName_ = currency.fullName;
        }
      }
      if (selectedCurrencyId_)
      {
        if (selectedCurrencyId_.get() == currency.currencyId)
        {
          ui_.currencyEdit->setText(currency.fullName);
          updateWidgets();
        }            
      }      
    }

    void PaysheetCardPage::on_database_currencyRemoved(const ma::chrono::model::Currency& currency)
    {
      if (selectedCurrencyId_)
      {
        if (selectedCurrencyId_.get() == currency.currencyId)
        {
          ui_.currencyEdit->setText(QString());
          selectedCurrencyId_.reset();
          updateWidgets();
        }
      }
    }

  } // namespace chrono
} // namespace ma
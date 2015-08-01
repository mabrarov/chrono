/*
TRANSLATOR ma::chrono::LcreditCardPage
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
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/widgetutility.h>
#include <ma/chrono/sqlutility.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/counterpart.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/localeutility.h>
#include <ma/chrono/lcreditcardpage.h>

namespace ma
{
  namespace chrono
  {    
    LcreditCardPage::LcreditCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent)
      : CardPage(resourceManager, taskWindowManager, databaseModel, helpAction, lcreditEntity, mode, entityId, parent)
      , dataAwareWidgets_()
      , contentsChanged_(false)     
    {
      ui_.setupUi(this);          
      ui_.tabWidget->setCurrentIndex(0);
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      LocaleUtility::setupLocaleAwareWidget(ui_.lcreditDate);
      LocaleUtility::setupLocaleAwareWidget(ui_.endDate);      
      createAction()->setText(tr("&New letter of credit"));
      dataAwareWidgets_ << ui_.lcreditNumberEdit << ui_.lcreditDate << ui_.endDate << ui_.remarkTextEdit;
      requiredMarkers_  << ui_.asterikLabel << ui_.asterikLabel_2 
                        << ui_.asterikLabel_3 << ui_.asterikLabel_4;
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
      QObject::connect(databaseModel.get(), SIGNAL(counterpartUpdated(const ma::chrono::model::Counterpart&)), SLOT(on_database_counterpartUpdated(const ma::chrono::model::Counterpart&)));                  
      updateWidgets();      
    }

    LcreditCardPage::~LcreditCardPage()
    {
    } 

    void LcreditCardPage::refresh()
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

    void LcreditCardPage::save()
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

    void LcreditCardPage::cancel()
    {
      if (editMode == mode())
      {      
        populateWidgets();
      }      
      CardPage::cancel();
      updateWidgets();
    }

    void LcreditCardPage::edit()
    {      
      CardPage::edit();
      updateWidgets();
    }

    void LcreditCardPage::remove()
    {
      if (createMode != mode())
      {
        if (removeFromDatabase())
        {
          CardPage::remove();
        }
      }            
    }

    std::auto_ptr<CardPage> LcreditCardPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new LcreditCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }

    void LcreditCardPage::connectDataAwareWidgets()
    {
      
      typedef QWidgetList::const_iterator const_iterator;
      
      for (const_iterator it = dataAwareWidgets_.begin(), end = dataAwareWidgets_.end(); it != end; ++it)
      {
        QWidget* widget = *it;        
        if (QLineEdit* edit = qobject_cast<QLineEdit*>(widget))
        {
          QObject::connect(edit, SIGNAL(textEdited(const QString&)), SLOT(on_lineEdit_textEdited(const QString&)));
        }
        else if (QTextEdit* edit = qobject_cast<QTextEdit*>(widget))
        {
          QObject::connect(edit, SIGNAL(textChanged()), SLOT(on_textEdit_textChanged()));
        }        
        else if (QDateEdit* edit = qobject_cast<QDateEdit*>(widget))
        {
          QObject::connect(edit, SIGNAL(dateChanged(const QDate&)), SLOT(on_dateEdit_dateChanged(const QDate&)));
        }
      }
    }

    void LcreditCardPage::updateWidgets()
    {      
      
      typedef QWidgetList::const_iterator const_iterator;

      Mode currentMode = mode();
      bool readOnly = viewMode == currentMode;
      for (const_iterator it = dataAwareWidgets_.begin(), end = dataAwareWidgets_.end(); it != end; ++it)
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
          updateWindowTitle(tr("Letters of credit - %1").arg(entityData_.lcreditNumber));
        }
        showRequiredMarkers(false);
        break;

      case editMode:
        if (entityId())
        {
          if (contentsChanged_)
          {
            updateWindowTitle(tr("Letters of credit - %1* - edit").arg(entityData_.lcreditNumber));
          }
          else
          {
            updateWindowTitle(tr("Letters of credit - %1 - edit").arg(entityData_.lcreditNumber));
          }        
        }
        showRequiredMarkers(true);
        break;

      case createMode:
        updateWindowTitle(tr("Letters of credit - New letter of credit creation*"));
        showRequiredMarkers(true);
        break;
      }
    }

    bool LcreditCardPage::populateFromWidgets()
    {
      static const char* lcreditNumberTitle = QT_TR_NOOP("Number of letter of credit");            
      static const char* contractTitle = QT_TR_NOOP("Contract");            
      static const char* remarkTitle = QT_TR_NOOP("Remark");      
      
      static const char* messageTitle = QT_TR_NOOP("Invalid data input");
      static const char* notEmpty = QT_TR_NOOP("Field \"%1\" can\'t be empty.");
      static const char* tooLongText = QT_TR_NOOP("Field \"%1\" can\'t exceed %2 characters.");      
            
      int contentsTabIndex = ui_.tabWidget->indexOf(ui_.contentsTab);
      bool validated;
      // Get LCredit number
      QString lcreditNumber = ui_.lcreditNumberEdit->displayText();            
      if (lcreditNumber.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(lcreditNumberTitle)));
      } 
      else if (lcreditNumber.length() > model::Lcredit::lcreditNumberMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(lcreditNumberTitle)).arg(model::Lcredit::lcreditNumberMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.lcreditNumberEdit->setFocus(Qt::TabFocusReason);
        ui_.lcreditNumberEdit->selectAll();
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
      // Get remark      
      QString remark = ui_.remarkTextEdit->toPlainText().trimmed();
      if (remark.length() > model::Lcredit::remarkMaxLen)
      {
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(remarkTitle)).arg(model::Lcredit::remarkMaxLen));        
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.remarkTextEdit->setFocus(Qt::TabFocusReason);
        ui_.remarkTextEdit->selectAll();
        return false;
      }
                      
      entityData_.lcreditNumber = lcreditNumber;
      entityData_.contractId    = selectedContractId_.get();      
      entityData_.lcreditDate   = ui_.lcreditDate->date();
      entityData_.endDate       = ui_.endDate->date();
      entityData_.remark = remark.isEmpty() ? OptionalQString() : remark;
      return true;      
    }

    bool LcreditCardPage::populateFromDatabase()
    {      
      static const char* querySql = "select " \
        "cr.\"NUMBER\", cntp.\"SHORT_NAME\", " \
        "t.\"CONTRACT_ID\", cr.\"COUNTERPART_ID\", " \
        "t.\"NUMBER\", t.\"LCREDIT_DATE\", t.\"END_DATE\", t.\"REMARK\", " \
        "cu.\"LOGIN\", uu.\"LOGIN\", " \
        "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", " \
        "t.\"CREATE_USER_ID\", t.\"UPDATE_USER_ID\" " \
        "from \"LCREDIT\" t " \
        "join \"CONTRACT\" cr on t.\"CONTRACT_ID\" = cr.\"CONTRACT_ID\" " \
        "join \"COUNTERPART\" cntp on cr.\"COUNTERPART_ID\" = cntp.\"COUNTERPART_ID\" " \
        "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
        "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
        "where t.\"LCREDIT_ID\" = ?";

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
              entityData_.lcreditId  = entityId().get();
              contractNumber_        = query.value(0).toString();
              counterpartShortName_  = query.value(1).toString();
              entityData_.contractId = query.value(2).toLongLong();
              counterpartId_            = query.value(3).toLongLong();
              entityData_.lcreditNumber = query.value(4).toString();
              entityData_.lcreditDate   = databaseModel()->convertFromServer(query.value(5).toDate());
              entityData_.endDate       = databaseModel()->convertFromServer(query.value(6).toDate());
              entityData_.remark = query.isNull(7) ? OptionalQString() : query.value(7).toString();
              createUserName_          = query.value(8).toString(); 
              updateUserName_          = query.value(9).toString(); 
              entityData_.createTime   = databaseModel()->convertFromServer(query.value(10).toDateTime()); 
              entityData_.updateTime   = databaseModel()->convertFromServer(query.value(11).toDateTime());
              entityData_.createUserId = query.value(12).toLongLong();
              entityData_.updateUserId = query.value(13).toLongLong();
              return true;
            }
          }
        }
      }
      return false;
    }

    void LcreditCardPage::populateWidgets()
    {
      if (entityId())
      { 
        // Contents page
        ui_.lcreditNumberEdit->setText(entityData_.lcreditNumber);      
        ui_.lcreditDate->setDate(entityData_.lcreditDate);
        ui_.endDate->setDate(entityData_.endDate);
        selectedContractId_ = entityData_.contractId;
        ui_.contractEdit->setText(contractNumber_);
        selectedCounterpartId_ = counterpartId_;
        ui_.counterpartEdit->setText(counterpartShortName_);
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

    void LcreditCardPage::populateExportData()
    {
      static QString numberCaption = tr("Number of letter of credit");
      static QString lcreditDateCaption = tr("Date of letter of credit");
      static QString endDateCaption = tr("End date");
      static QString contractNumberCaption = tr("Contract number");      
      static QString counterpartShortNameCaption = tr("Counterpart");      
      static QString remarksCaption = tr("Remarks");      
      static QString createUserCaption = tr("User, creator");
      static QString createTimeCaption = tr("Time, created");
      static QString updateUserCaption = tr("User, last update");
      static QString updateTimeCaption = tr("Time, last update");                    
        
      ExportDataList exportDataList;
      exportDataList << std::make_pair(numberCaption, QVariant(entityData_.lcreditNumber));
      exportDataList << std::make_pair(lcreditDateCaption, QVariant(entityData_.lcreditDate));
      exportDataList << std::make_pair(endDateCaption, QVariant(entityData_.endDate));
      exportDataList << std::make_pair(contractNumberCaption, QVariant(contractNumber_));
      exportDataList << std::make_pair(counterpartShortNameCaption, QVariant(counterpartShortName_));
      exportDataList << std::make_pair(remarksCaption, entityData_.remark ? QVariant(entityData_.remark.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(createUserCaption, QVariant(createUserName_));
      exportDataList << std::make_pair(createTimeCaption, QVariant(entityData_.createTime));
      exportDataList << std::make_pair(updateUserCaption, QVariant(updateUserName_));
      exportDataList << std::make_pair(updateTimeCaption, QVariant(entityData_.updateTime));      

      setExportDataList(exportDataList);
    }

    bool LcreditCardPage::updateDatabase()
    {
      static const char* querySql = "update \"LCREDIT\" set " \
        "\"NUMBER\" = ?, \"LCREDIT_DATE\" = ?, \"END_DATE\" = ?, \"CONTRACT_ID\" = ?, " \
        "\"REMARK\" = ?, " \
        "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\' " \
        "where \"LCREDIT_ID\" = ?";

      if (entityId())
      {        
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {          
          query.bindValue(0, entityData_.lcreditNumber);
          query.bindValue(1, entityData_.lcreditDate);
          query.bindValue(2, entityData_.endDate);
          query.bindValue(3, entityData_.contractId);          
          query.bindValue(4, entityData_.remark ? entityData_.remark.get() : QVariant(QVariant::String));          
          query.bindValue(5, databaseModel()->userId());
          query.bindValue(6, entityData_.lcreditId);
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

    bool LcreditCardPage::insertIntoDatabase()
    {
      static const char* querySql = "insert into \"LCREDIT\" (" \
        "\"LCREDIT_ID\", \"NUMBER\", \"LCREDIT_DATE\", \"END_DATE\", \"CONTRACT_ID\", " \
        "\"REMARK\", \"CREATE_USER_ID\", \"UPDATE_USER_ID\", " \
        "\"CREATE_TIME\", \"UPDATE_TIME\") " \
        "values(?, ?, ?, ?, ?, ?, ?, ?, \'NOW\', \'NOW\')";            

      if (OptionalQInt64 newEntityId = databaseModel()->generateId(lcreditEntity))
      {
        entityData_.lcreditId = newEntityId.get();    
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.lcreditId);
          query.bindValue(1, entityData_.lcreditNumber);
          query.bindValue(2, databaseModel()->convertToServer(entityData_.lcreditDate));
          query.bindValue(3, databaseModel()->convertToServer(entityData_.endDate));
          query.bindValue(4, entityData_.contractId);
          query.bindValue(5, entityData_.remark ? entityData_.remark.get() : QVariant(QVariant::String));      
          query.bindValue(6, databaseModel()->userId());
          query.bindValue(7, databaseModel()->userId());      
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

    bool LcreditCardPage::removeFromDatabase()
    {
      static const char* querySql = "delete from \"LCREDIT\" where \"LCREDIT_ID\" = ?";

      if (QMessageBox::Yes == QMessageBox::question(this, tr("Letters of credit"), 
        tr("Delete letter of credit?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
      {
        if (entityId())
        {
          QSqlQuery query(databaseModel()->database());              
          if (query.prepare(querySql))
          {
            query.bindValue(0, entityData_.lcreditId);
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

    void LcreditCardPage::on_lineEdit_textEdited(const QString&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void LcreditCardPage::on_textEdit_textChanged()
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void LcreditCardPage::on_dateEdit_dateChanged(const QDate&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }    

    void LcreditCardPage::on_contractBtn_clicked(bool /*checked*/)
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
          updateContractRelatedNonPersistData(selectedContractId_.get());
          contentsChanged_ = true;
          updateWidgets();
        }
      } 
    }
    
    void LcreditCardPage::on_counterpartBtn_clicked(bool /*checked*/)
    {
      if (selectedCounterpartId_)
      {
        showCounterpartCardTaskWindow(selectedCounterpartId_.get());
      }
    }    

    void LcreditCardPage::updateContractRelatedPersistData(qint64 contractId)
    {
      static const char* querySql = "select " \
        "cr.\"NUMBER\", cntp.\"SHORT_NAME\", " \
        "cr.\"COUNTERPART_ID\" " \
        "from \"CONTRACT\" cr " \
        "join \"COUNTERPART\" cntp on cr.\"COUNTERPART_ID\" = cntp.\"COUNTERPART_ID\" " \
        "where cr.\"CONTRACT_ID\" = ?";
      
      if (entityId())
      {
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {          
          query.bindValue(0, contractId);
          if (query.exec())
          {
            if (query.next())
            {   
              contractNumber_ = query.value(0).toString();
              counterpartShortName_ = query.value(1).toString();
              counterpartId_ = query.value(2).toLongLong();      
            }
          }
        }            
      }
    }

    void LcreditCardPage::updateContractRelatedNonPersistData(qint64 contactId)
    {
      static const char* querySql = "select " \
        "cr.\"NUMBER\", cntp.\"SHORT_NAME\", " \
        "cr.\"COUNTERPART_ID\" " \
        "from \"CONTRACT\" cr " \
        "join \"COUNTERPART\" cntp on cr.\"COUNTERPART_ID\" = cntp.\"COUNTERPART_ID\" " \
        "where cr.\"CONTRACT_ID\" = ?";

      if (selectedContractId_)
      {
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {          
          query.bindValue(0, contactId);          
          if (query.exec())
          {
            if (query.next())
            {   
              ui_.contractEdit->setText(query.value(0).toString());
              ui_.counterpartEdit->setText(query.value(1).toString());
              selectedCounterpartId_.reset(query.value(2).toLongLong());
            }
          }
        }            
      }
    }

    void LcreditCardPage::showRequiredMarkers(bool visible)
    {
      WidgetUtility::show(requiredMarkers_, visible);
    }

    void LcreditCardPage::on_createUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.createUserId);
      }        
    }

    void LcreditCardPage::on_updateUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.updateUserId);
      }        
    }

    void LcreditCardPage::on_database_userUpdated(const ma::chrono::model::User& user)
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

    void LcreditCardPage::on_database_contractUpdated(const ma::chrono::model::Contract& contract)
    {
      if (entityId())
      {        
        if (entityData_.contractId == contract.contractId)
        {
          updateContractRelatedPersistData(contract.contractId);
        }
      }
      if (selectedContractId_)
      {
        if (selectedContractId_.get() == contract.contractId)
        {
          if (selectedContractId_.get() == entityData_.contractId)
          {
            ui_.contractEdit->setText(contractNumber_);            
            selectedCounterpartId_ = counterpartId_;
            ui_.counterpartEdit->setText(counterpartShortName_);         
          }
          else
          {
            updateContractRelatedNonPersistData(contract.contractId);
          }
          updateWidgets();
        }
      }      
    }

    void LcreditCardPage::on_database_contractRemoved(const ma::chrono::model::Contract& contract)
    {
      if (selectedContractId_)
      {
        if (selectedContractId_.get() == contract.contractId)
        {
          ui_.contractEdit->setText(QString());
          selectedContractId_.reset();              
          selectedCounterpartId_.reset();
          ui_.counterpartEdit->setText(QString());
          updateWidgets();
        }
      }
    }
    
    void LcreditCardPage::on_database_counterpartUpdated(const ma::chrono::model::Counterpart& counterpart)
    {
      if (entityId())
      {
        if (counterpartId_ == counterpart.counterpartId)
        {          
          counterpartShortName_ = counterpart.shortName;
        }
      }
      if (selectedCounterpartId_)
      {
        if (selectedCounterpartId_.get() == counterpart.counterpartId)
        {
          ui_.counterpartEdit->setText(counterpart.shortName);          
          updateWidgets();
        }
      }
    }    

  } // namespace chrono
} // namespace ma
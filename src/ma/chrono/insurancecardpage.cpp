/*
TRANSLATOR ma::chrono::InsuranceCardPage
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
#include <ma/chrono/constants.h>
#include <ma/chrono/dbconstants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/listselectdialog.h>
#include <ma/chrono/currencylistpage.h>
#include <ma/chrono/contractlistpage.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/widgetutility.h>
#include <ma/chrono/sqlutility.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/currency.h>
#include <ma/chrono/model/currencyamount.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/localeutility.h>
#include <ma/chrono/insurancecardpage.h>

namespace ma
{
  namespace chrono
  {    
    InsuranceCardPage::InsuranceCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent)
      : CardPage(resourceManager, taskWindowManager, databaseModel, helpAction, insuranceEntity, mode, entityId, parent)
      , dataAwareWidgets_()
      , contentsChanged_(false)
    {
      ui_.setupUi(this);      
      ui_.tabWidget->setCurrentIndex(0);
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      LocaleUtility::setupLocaleAwareWidget(ui_.signingDateEdit);
      LocaleUtility::setupLocaleAwareWidget(ui_.startDateEdit);
      LocaleUtility::setupLocaleAwareWidget(ui_.endDateEdit);      
      createAction()->setText(tr("&New insurance"));
      dataAwareWidgets_ << ui_.policyNumberEdit << ui_.signingDateEdit << ui_.startDateEdit 
                        << ui_.endDateEdit << ui_.amountEdit;
      requiredMarkers_  << ui_.asterikLabel << ui_.asterikLabel_2 << ui_.asterikLabel_3 
                        << ui_.asterikLabel_4 << ui_.asterikLabel_5 << ui_.asterikLabel_6 
                        << ui_.asterikLabel_7;
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
      QObject::connect(databaseModel.get(), SIGNAL(currencyUpdated(const ma::chrono::model::Currency&)), SLOT(on_database_currencyUpdated(const ma::chrono::model::Currency&)));
      QObject::connect(databaseModel.get(), SIGNAL(currencyRemoved(const ma::chrono::model::Currency&)), SLOT(on_database_currencyRemoved(const ma::chrono::model::Currency&)));      
      QObject::connect(databaseModel.get(), SIGNAL(contractUpdated(const ma::chrono::model::Contract&)), SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));
      QObject::connect(databaseModel.get(), SIGNAL(contractRemoved(const ma::chrono::model::Contract&)), SLOT(on_database_contractRemoved(const ma::chrono::model::Contract&)));      
      updateWidgets();
    }

    InsuranceCardPage::~InsuranceCardPage()
    {
    } 

    void InsuranceCardPage::refresh()
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

    void InsuranceCardPage::save()
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

    void InsuranceCardPage::cancel()
    {
      if (editMode == mode())
      {      
        populateWidgets();
      }      
      CardPage::cancel();
      updateWidgets();
    }

    void InsuranceCardPage::edit()
    {      
      CardPage::edit();
      updateWidgets();
    }

    void InsuranceCardPage::remove()
    {
      if (createMode != mode())
      {
        if (removeFromDatabase())
        {
          CardPage::remove();
        }
      }            
    }

    std::auto_ptr<CardPage> InsuranceCardPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new InsuranceCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }

    void InsuranceCardPage::setSelectedContractId(qint64 contractId)
    {
      selectedContractId_ = contractId;            
      updateContractRelatedNonPersistData(contractId);
    }

    void InsuranceCardPage::connectDataAwareWidgets()
    {
      
      typedef QWidgetList::const_iterator const_iterator;
      
      for (const_iterator it = dataAwareWidgets_.begin(), end = dataAwareWidgets_.end(); it != end; ++it)
      {
        QWidget* widget = *it;        
        if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget))
        {
          QObject::connect(lineEdit, SIGNAL(textEdited(const QString&)), SLOT(on_lineEdit_textEdited(const QString&)));
        }        
        else if (QDateEdit* dateEdit = qobject_cast<QDateEdit*>(widget))
        {
          QObject::connect(dateEdit, SIGNAL(dateChanged(const QDate&)), SLOT(on_dateEdit_dateChanged(const QDate&)));
        }
      }
    }

    void InsuranceCardPage::updateWidgets()
    {      
      
      typedef QWidgetList::const_iterator const_iterator;

      Mode currentMode = mode();
      bool readOnly = viewMode == currentMode;
      WidgetUtility::setReadOnly(dataAwareWidgets_, readOnly);

      bool peristance = createMode != currentMode && entityId();      
      int generalTabIndex = ui_.tabWidget->indexOf(ui_.generalTab);      
      ui_.tabWidget->setTabEnabled(generalTabIndex, peristance);            
      switch (currentMode)
      {
      case viewMode:
        if (entityId())
        {
          updateWindowTitle(tr("Insurances - %1").arg(entityData_.policyNumber));
        }
        showRequiredMarkers(false);
        break;

      case editMode:
        if (entityId())
        {
          if (contentsChanged_)
          {
            updateWindowTitle(tr("Insurances - %1* - edit").arg(entityData_.policyNumber));
          }
          else
          {
            updateWindowTitle(tr("Insurances - %1 - edit").arg(entityData_.policyNumber));
          }        
        }
        showRequiredMarkers(true);
        break;

      case createMode:
        updateWindowTitle(tr("Insurances - New insurance creation*"));
        showRequiredMarkers(true);
        break;
      }
    }

    bool InsuranceCardPage::populateFromWidgets()
    {
      static const char* policyNumberTitle = QT_TR_NOOP("Policy number");      
      static const char* amountTitle = QT_TR_NOOP("Amount");
      static const char* currencyTitle = QT_TR_NOOP("Currency");
      static const char* contractTitle = QT_TR_NOOP("Contract");
      
      static const char* messageTitle = QT_TR_NOOP("Invalid data input");
      static const char* notEmpty = QT_TR_NOOP("Field \"%1\" can\'t be empty.");
      static const char* tooLongText = QT_TR_NOOP("Field \"%1\" can\'t exceed %2 characters.");      
      static const char* invalidFieldValueText = QT_TR_NOOP("Invalid value in field \"%1\".");      

      int contentsTabIndex = ui_.tabWidget->indexOf(ui_.contentsTab);
      bool validated;
      // Get policyNumber
      QString policyNumber = ui_.policyNumberEdit->text().trimmed();
      if (policyNumber.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(policyNumberTitle)));
      } 
      else if (policyNumber.length() > model::Insurance::policyNumberMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(policyNumberTitle)).arg(model::Insurance::policyNumberMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.policyNumberEdit->setFocus(Qt::TabFocusReason);
        ui_.policyNumberEdit->selectAll();
        return false;
      } 
      if (!selectedContractId_)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(contractTitle)));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.contractBtn->setFocus(Qt::TabFocusReason);
        return false;
      }
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
      if (!currencyAmount.setValue(ui_.amountEdit->text()))
      {
        QMessageBox::warning(this, tr(messageTitle), tr(invalidFieldValueText).arg(tr(amountTitle)));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.amountEdit->setFocus(Qt::TabFocusReason);
        ui_.amountEdit->selectAll();
        return false;
      }
      entityData_.contractId   = selectedContractId_.get();
      entityData_.currencyId   = selectedCurrencyId_.get();
      entityData_.policyNumber = policyNumber;      
      entityData_.amount = currencyAmount.amount();
      entityData_.signingDate = ui_.signingDateEdit->date();
      entityData_.startDate = ui_.startDateEdit->date();
      entityData_.endDate = ui_.endDateEdit->date();      
      return true;
    }

    bool InsuranceCardPage::populateFromDatabase()
    {      
      static const char* querySql = "select " \
        "t.\"POLICY_NUMBER\", " \
        "cr.\"NUMBER\", t.\"AMOUNT\", cur.\"FULL_NAME\", " \
        "t.\"SIGNING_DATE\", t.\"START_DATE\", t.\"END_DATE\", " \
        "t.\"CREATE_USER_ID\", cu.\"LOGIN\", " \
        "t.\"UPDATE_USER_ID\", uu.\"LOGIN\", " \
        "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", " \
        "t.\"CONTRACT_ID\", t.\"CURRENCY_ID\" " \
        "from \"INSURANCE\" t " \
        "join \"CONTRACT\" cr on t.\"CONTRACT_ID\" = cr.\"CONTRACT_ID\" " \
        "join \"CURRENCY\" cur on t.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
        "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
        "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
        "where t.\"INSURANCE_ID\" = ?";

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
              entityData_.insuranceId  = entityId().get();
              entityData_.policyNumber = query.value(0).toString();
              contractNumber_    = query.value(1).toString();
              entityData_.amount = query.value(2).toLongLong();
              currencyFullName_  = query.value(3).toString();
              entityData_.signingDate = databaseModel()->convertFromServer(query.value(4).toDate());
              entityData_.startDate   = databaseModel()->convertFromServer(query.value(5).toDate());
              entityData_.endDate     = databaseModel()->convertFromServer(query.value(6).toDate());              
              entityData_.createUserId = query.value(7).toLongLong();
              createUserName_          = query.value(8).toString(); 
              entityData_.updateUserId = query.value(9).toLongLong();
              updateUserName_          = query.value(10).toString();               
              entityData_.createTime   = databaseModel()->convertFromServer(query.value(11).toDateTime()); 
              entityData_.updateTime   = databaseModel()->convertFromServer(query.value(12).toDateTime());
              entityData_.contractId = query.value(13).toLongLong();
              entityData_.currencyId = query.value(14).toLongLong();
              return true;
            }
          }
        }
      }
      return false;
    }

    void InsuranceCardPage::populateWidgets()
    {
      if (entityId())
      {
        // Contents page
        ui_.policyNumberEdit->setText(entityData_.policyNumber);        
        ui_.signingDateEdit->setDate(entityData_.signingDate);
        ui_.startDateEdit->setDate(entityData_.startDate);
        ui_.endDateEdit->setDate(entityData_.endDate);
        ui_.currencyEdit->setText(currencyFullName_);
        ui_.amountEdit->setText(model::CurrencyAmount(entityData_.amount).toString());
        ui_.contractNumberEdit->setText(contractNumber_);
        selectedCurrencyId_ = entityData_.currencyId;
        selectedContractId_ = entityData_.contractId;
        // General data page
        ui_.createTimeEdit->setText(entityData_.createTime.toString(Qt::DefaultLocaleShortDate));
        ui_.updateTimeEdit->setText(entityData_.updateTime.toString(Qt::DefaultLocaleShortDate));        
        ui_.createUserEdit->setText(createUserName_);
        ui_.updateUserEdit->setText(updateUserName_);        
        populateExportData();
        contentsChanged_ = false;
      }
    } 

    void InsuranceCardPage::populateExportData()
    {      
      static const QString policyNumberCaption = tr("Policy number");      
      static const QString contractNumberCaption = tr("Contract");
      static const QString signDateCaption = tr("Signinig date");
      static const QString startDateCaption = tr("Start date");
      static const QString endDateCaption = tr("End date");
      static const QString amountCaption = tr("Amount");
      static const QString currencyFullNameCaption = tr("Currency");     
      static const QString createUserCaption = tr("User, creator");
      static const QString createTimeCaption = tr("Time, created");
      static const QString updateUserCaption = tr("User, last update");
      static const QString updateTimeCaption = tr("Time, last update");
        
      ExportDataList exportDataList;
      exportDataList << std::make_pair(policyNumberCaption, QVariant(entityData_.policyNumber));      
      exportDataList << std::make_pair(contractNumberCaption, QVariant(contractNumber_));
      exportDataList << std::make_pair(signDateCaption, QVariant(entityData_.signingDate));
      exportDataList << std::make_pair(startDateCaption, QVariant(entityData_.startDate));
      exportDataList << std::make_pair(endDateCaption, QVariant(entityData_.endDate));
      exportDataList << std::make_pair(amountCaption, QVariant::fromValue(model::CurrencyAmount(entityData_.amount)));
      exportDataList << std::make_pair(currencyFullNameCaption, QVariant(currencyFullName_));            
      exportDataList << std::make_pair(createUserCaption, QVariant(createUserName_));
      exportDataList << std::make_pair(createTimeCaption, QVariant(entityData_.createTime));
      exportDataList << std::make_pair(updateUserCaption, QVariant(updateUserName_));
      exportDataList << std::make_pair(updateTimeCaption, QVariant(entityData_.updateTime));      
            
      setExportDataList(exportDataList);
    }

    bool InsuranceCardPage::updateDatabase()
    {      
      static const char* querySql = "update \"INSURANCE\" set " \
        "\"CONTRACT_ID\" = ?, \"CURRENCY_ID\" = ?, " \
        "\"POLICY_NUMBER\" = ?, \"AMOUNT\" = ?, " \
        "\"SIGNING_DATE\" = ?, \"START_DATE\" = ?, \"END_DATE\" = ?, " \
        "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\' " \
        "where \"INSURANCE_ID\" = ?";      

      if (entityId())
      {
        QSqlQuery query(databaseModel()->database());
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.contractId);
          query.bindValue(1, entityData_.currencyId);
          query.bindValue(2, entityData_.policyNumber);
          query.bindValue(3, entityData_.amount);
          query.bindValue(4, databaseModel()->convertToServer(entityData_.signingDate));
          query.bindValue(5, databaseModel()->convertToServer(entityData_.startDate));
          query.bindValue(6, databaseModel()->convertToServer(entityData_.endDate));
          query.bindValue(7, databaseModel()->userId());
          query.bindValue(8, entityData_.insuranceId);
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

    bool InsuranceCardPage::insertIntoDatabase()
    {
      static const char* querySql = "insert into \"INSURANCE\"(" \
        "\"INSURANCE_ID\", \"CONTRACT_ID\", \"CURRENCY_ID\", " \
        "\"POLICY_NUMBER\", \"AMOUNT\", " \
        "\"SIGNING_DATE\", \"START_DATE\", \"END_DATE\", " \
        "\"CREATE_USER_ID\", \"UPDATE_USER_ID\", \"CREATE_TIME\", \"UPDATE_TIME\") " \
        "values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, \'NOW\', \'NOW\')";
      
      if (OptionalQInt64 newEntityId = databaseModel()->generateId(insuranceEntity))
      {
        entityData_.insuranceId = newEntityId.get();
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.insuranceId);
          query.bindValue(1, entityData_.contractId);
          query.bindValue(2, entityData_.currencyId);
          query.bindValue(3, entityData_.policyNumber);
          query.bindValue(4, entityData_.amount);
          query.bindValue(5, databaseModel()->convertToServer(entityData_.signingDate));
          query.bindValue(6, databaseModel()->convertToServer(entityData_.startDate));
          query.bindValue(7, databaseModel()->convertToServer(entityData_.endDate));
          query.bindValue(8, databaseModel()->userId());          
          query.bindValue(9, databaseModel()->userId());
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

    bool InsuranceCardPage::removeFromDatabase()
    {
      static const char* querySql = "delete from \"INSURANCE\" where \"INSURANCE_ID\" = ?";

      if (QMessageBox::Yes == QMessageBox::question(this, tr("Insurances"), 
        tr("Delete insurance?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
      {
        if (entityId())
        {
          QSqlQuery query(databaseModel()->database());              
          if (query.prepare(querySql))
          {
            query.bindValue(0, entityData_.insuranceId);
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

    void InsuranceCardPage::updateContractRelatedNonPersistData(qint64 contractId)
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
              ui_.contractNumberEdit->setText(query.value(0).toString());              
            }
          }
        }
      }
    }

    void InsuranceCardPage::showRequiredMarkers(bool visible)
    {
      WidgetUtility::show(requiredMarkers_, visible);
    }    

    void InsuranceCardPage::on_lineEdit_textEdited(const QString&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }    

    void InsuranceCardPage::on_dateEdit_dateChanged(const QDate&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }    

    void InsuranceCardPage::on_database_userUpdated(const ma::chrono::model::User& user)
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

    void InsuranceCardPage::on_database_currencyUpdated(const ma::chrono::model::Currency& currency)
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

    void InsuranceCardPage::on_database_currencyRemoved(const ma::chrono::model::Currency& currency)
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

    void InsuranceCardPage::on_database_contractUpdated(const ma::chrono::model::Contract& contract)
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
          ui_.contractNumberEdit->setText(contract.number);
          updateWidgets();
        }
      }
    }

    void InsuranceCardPage::on_database_contractRemoved(const ma::chrono::model::Contract& contract)
    {
      if (selectedContractId_)
      {
        if (selectedContractId_.get() == contract.contractId)
        {          
          ui_.contractNumberEdit->setText(QString());
          selectedContractId_.reset();
          updateWidgets();
        }
      }
    }    

    void InsuranceCardPage::on_currencyBtn_clicked(bool /*checked*/)
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
          if (OptionalQString currencyFullName = listSelectDialog.listPage()->selectedRelationalText())
          {
            ui_.currencyEdit->setText(currencyFullName.get());
          }
          contentsChanged_ = true;
          updateWidgets();
        }
      }      
    }

    void InsuranceCardPage::on_contractBtn_clicked(bool /*checked*/)
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
          OptionalInt(), UserDefinedFilter(), UserDefinedOrder(),  0));
        ListSelectDialog listSelectDialog(this, listPage);
        listSelectDialog.setWindowModality(Qt::WindowModal);
        if (QDialog::Accepted == listSelectDialog.exec())
        {
          selectedContractId_ = listSelectDialog.listPage()->selectedId();          
          if (OptionalQString contractNumber = listSelectDialog.listPage()->selectedRelationalText())
          {
            ui_.contractNumberEdit->setText(contractNumber.get());
          }
          contentsChanged_ = true;
          updateWidgets();
        }
      }      
    }    

    void InsuranceCardPage::on_createUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.createUserId);
      }        
    }

    void InsuranceCardPage::on_updateUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.updateUserId);
      }        
    }    

  } // namespace chrono
} // namespace ma
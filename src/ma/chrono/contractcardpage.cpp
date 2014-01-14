/*
TRANSLATOR ma::chrono::ContractCardPage
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QCalendarWidget>
#include <QVBoxLayout>
#include <QSqlQuery>
#include <QSqlError>
#include <ma/chrono/constants.h>
#include <ma/chrono/dbconstants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/listselectdialog.h>
#include <ma/chrono/currencylistpage.h>
#include <ma/chrono/counterpartlistpage.h>
#include <ma/chrono/banklistpage.h>
#include <ma/chrono/residentlistpage.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/widgetutility.h>
#include <ma/chrono/sqlutility.h>
#include <ma/chrono/model/currencyamount.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/counterpart.h>
#include <ma/chrono/model/bank.h>
#include <ma/chrono/model/resident.h>
#include <ma/chrono/model/currency.h>
#include <ma/chrono/localeutility.h>
#include <ma/chrono/buildenvironment.h>
#include <ma/chrono/actionutility.h>
#include <ma/chrono/contractccdlistpage.h>
#include <ma/chrono/contractinsurancelistpage.h>
#include <ma/chrono/contractpaysheetlistpage.h>
#include <ma/chrono/contractsbollistpage.h>
#include <ma/chrono/contractbollistpage.h>
#include <ma/chrono/contractcardpage.h>

namespace ma
{
  namespace chrono
  {    
    ContractCardPage::ContractCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent)
      : CardPage(resourceManager, taskWindowManager, databaseModel, helpAction, contractEntity, mode, entityId, parent)
      , dataAwareWidgets_()
      , contentsChanged_(false)      
    {
      ui_.setupUi(this);

      QVBoxLayout* ccdTabLayout = new QVBoxLayout(ui_.ccdTab);           
      contractCcdListPage_ = new ContractCcdListPage(resourceManager,
        taskWindowManager, databaseModel, helpAction, entityId, UserDefinedFilter(), UserDefinedOrder(), ui_.ccdTab);
      ccdTabLayout->addWidget(contractCcdListPage_);

      QVBoxLayout* insuranceTabLayout = new QVBoxLayout(ui_.insuranceTab);           
      contractInsuranceListPage_ = new ContractInsuranceListPage(resourceManager,
        taskWindowManager, databaseModel, helpAction, entityId, UserDefinedFilter(), UserDefinedOrder(), ui_.insuranceTab);
      insuranceTabLayout->addWidget(contractInsuranceListPage_);

      QVBoxLayout* paysheetTabLayout = new QVBoxLayout(ui_.paysheetTab);           
      contractPaysheetListPage_ = new ContractPaysheetListPage(resourceManager,
        taskWindowManager, databaseModel, helpAction, entityId, UserDefinedFilter(), UserDefinedOrder(), ui_.paysheetTab);
      paysheetTabLayout->addWidget(contractPaysheetListPage_);

      QVBoxLayout* sbolTabLayout = new QVBoxLayout(ui_.sbolTab);           
      contractSbolListPage_ = new ContractSbolListPage(resourceManager,
        taskWindowManager, databaseModel, helpAction, entityId, UserDefinedFilter(), UserDefinedOrder(), ui_.sbolTab);
      sbolTabLayout->addWidget(contractSbolListPage_);

      QVBoxLayout* bolTabLayout = new QVBoxLayout(ui_.bolTab);           
      contractBolListPage_ = new ContractBolListPage(resourceManager,
        taskWindowManager, databaseModel, helpAction, entityId, UserDefinedFilter(), UserDefinedOrder(), ui_.bolTab);
      bolTabLayout->addWidget(contractBolListPage_);

      relatedDocsAction_ = new QAction(resourceManager->getIcon(bookmarksOrganizeIconName), tr("&Related docs"), this);
      QMenu* relatedDocsMenu = new QMenu(this);
      relatedDocsAction_->setMenu(relatedDocsMenu);

      {
        QAction* insuranceAction = new QAction(tr("&Insurances"), this);
        QMenu* insuranceMenu = new QMenu(this);          
        insuranceMenu->addActions(contractInsuranceListPage_->parentActions());
        insuranceAction->setMenu(insuranceMenu);
        relatedDocsMenu->addAction(insuranceAction);
      }

      {
        QAction* ccdAction = new QAction(tr("&CCD"), this);
        QMenu* ccdMenu = new QMenu(this);          
        ccdMenu->addActions(contractCcdListPage_->parentActions());
        ccdAction->setMenu(ccdMenu);
        relatedDocsMenu->addAction(ccdAction);      
      }

      {
        QAction* paysheetAction = new QAction(tr("&Pay sheets"), this);
        QMenu* paysheetMenu = new QMenu(this);          
        paysheetMenu->addActions(contractPaysheetListPage_->parentActions());
        paysheetAction->setMenu(paysheetMenu);
        relatedDocsMenu->addAction(paysheetAction);
      }

      {
        QAction* sbolAction = new QAction(tr("&SBOL"), this);
        QMenu* sbolMenu = new QMenu(this);          
        sbolMenu->addActions(contractSbolListPage_->parentActions());
        sbolAction->setMenu(sbolMenu);
        relatedDocsMenu->addAction(sbolAction);
      }

      {
        QAction* bolAction = new QAction(tr("&BOL"), this);
        QMenu* bolMenu = new QMenu(this);          
        bolMenu->addActions(contractBolListPage_->parentActions());
        bolAction->setMenu(bolMenu);
        relatedDocsMenu->addAction(bolAction);
      }

      QActionList additionalPrimaryAcions;
      additionalPrimaryAcions << relatedDocsAction_;      
      addPrimaryActions(additionalPrimaryAcions);

      ui_.passportNumberEdit->setText("00000000/0000/0000/1/0");      
      ui_.tabWidget->setCurrentIndex(0);
      ui_.closeStackedWidget->setCurrentIndex(0);
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      LocaleUtility::setupLocaleAwareWidget(ui_.signingDateEdit);
      LocaleUtility::setupLocaleAwareWidget(ui_.endDateEdit);
      LocaleUtility::setupLocaleAwareWidget(ui_.passportSigningDateEdit);
      LocaleUtility::setupLocaleAwareWidget(ui_.passportEndDateEdit);
      LocaleUtility::setupLocaleAwareWidget(ui_.closeDateEdit);      
      createAction()->setText(tr("&New contract"));
      dataAwareWidgets_ << ui_.contractNumberEdit << ui_.passportNumberEdit << ui_.closedCheckBox 
                        << ui_.signingDateEdit << ui_.endDateEdit << ui_.amountEdit
                        << ui_.closeRemarksEdit << ui_.remarksEdit << ui_.passportEndDateEdit
                        << ui_.closeDateEdit << ui_.passportSigningDateEdit;
      requiredMarkers_  << ui_.asterikLabel << ui_.asterikLabel_2 
                        << ui_.asterikLabel_3 << ui_.asterikLabel_4 << ui_.asterikLabel_5 
                        << ui_.asterikLabel_6 << ui_.asterikLabel_7 << ui_.asterikLabel_8
                        << ui_.asterikLabel_9 << ui_.asterikLabel_10 << ui_.asterikLabel_11;
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
      QObject::connect(databaseModel.get(), SIGNAL(counterpartUpdated(const ma::chrono::model::Counterpart&)), SLOT(on_database_counterpartUpdated(const ma::chrono::model::Counterpart&)));
      QObject::connect(databaseModel.get(), SIGNAL(counterpartRemoved(const ma::chrono::model::Counterpart&)), SLOT(on_database_counterpartRemoved(const ma::chrono::model::Counterpart&)));
      QObject::connect(databaseModel.get(), SIGNAL(bankUpdated(const ma::chrono::model::Bank&)), SLOT(on_database_bankUpdated(const ma::chrono::model::Bank&)));
      QObject::connect(databaseModel.get(), SIGNAL(bankRemoved(const ma::chrono::model::Bank&)), SLOT(on_database_bankRemoved(const ma::chrono::model::Bank&)));
      QObject::connect(databaseModel.get(), SIGNAL(residentUpdated(const ma::chrono::model::Resident&)), SLOT(on_database_residentUpdated(const ma::chrono::model::Resident&)));
      QObject::connect(databaseModel.get(), SIGNAL(residentRemoved(const ma::chrono::model::Resident&)), SLOT(on_database_residentRemoved(const ma::chrono::model::Resident&)));
      QObject::connect(contractCcdListPage_, SIGNAL(doubleClicked(const QModelIndex&)), SLOT(on_ccd_doubleClicked(const QModelIndex&)));
      QObject::connect(contractInsuranceListPage_, SIGNAL(doubleClicked(const QModelIndex&)), SLOT(on_insurance_doubleClicked(const QModelIndex&)));
      QObject::connect(contractPaysheetListPage_, SIGNAL(doubleClicked(const QModelIndex&)), SLOT(on_paysheet_doubleClicked(const QModelIndex&)));
      QObject::connect(contractSbolListPage_, SIGNAL(doubleClicked(const QModelIndex&)), SLOT(on_sbol_doubleClicked(const QModelIndex&)));
      QObject::connect(contractBolListPage_, SIGNAL(doubleClicked(const QModelIndex&)), SLOT(on_bol_doubleClicked(const QModelIndex&)));
      updateWidgets();
    }

    ContractCardPage::~ContractCardPage()
    {
    } 

    void ContractCardPage::refresh()
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

    void ContractCardPage::save()
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
              contractInsuranceListPage_->setContractId(entityId().get());
              contractCcdListPage_->setContractId(entityId().get());
              contractPaysheetListPage_->setContractId(entityId().get());
              contractSbolListPage_->setContractId(entityId().get());
              contractBolListPage_->setContractId(entityId().get());
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

    void ContractCardPage::cancel()
    {
      if (editMode == mode())
      {      
        populateWidgets();
      }      
      CardPage::cancel();
      updateWidgets();
    }

    void ContractCardPage::edit()
    {      
      CardPage::edit();
      updateWidgets();
    }

    void ContractCardPage::remove()
    {
      if (createMode != mode())
      {
        if (removeFromDatabase())
        {
          CardPage::remove();
        }
      }            
    }

    std::auto_ptr<CardPage> ContractCardPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new ContractCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }

    void ContractCardPage::connectDataAwareWidgets()
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
        else if (QCheckBox* edit = qobject_cast<QCheckBox*>(widget))
        {
          QObject::connect(edit, SIGNAL(stateChanged(int)), SLOT(on_checkBox_stateChanged(int)));
        }               
      }
    }

    void ContractCardPage::updateWidgets()
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
        else if (QAbstractSpinBox* edit = qobject_cast<QAbstractSpinBox*>(widget))
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
      int closeTabIndex = ui_.tabWidget->indexOf(ui_.closeTab);      
      int ccdTabIndex = ui_.tabWidget->indexOf(ui_.ccdTab);      
      int insuranceTabIndex = ui_.tabWidget->indexOf(ui_.insuranceTab);
      int paysheetTabIndex = ui_.tabWidget->indexOf(ui_.paysheetTab);
      int sbolTabIndex = ui_.tabWidget->indexOf(ui_.sbolTab);
      int bolTabIndex = ui_.tabWidget->indexOf(ui_.bolTab);
      int currentTabIndex = ui_.tabWidget->currentIndex();

      ui_.tabWidget->setTabEnabled(generalTabIndex, peristance);                  
      ui_.tabWidget->setTabEnabled(closeTabIndex, peristance);                  
      ui_.tabWidget->setTabEnabled(ccdTabIndex, peristance);      
      ui_.tabWidget->setTabEnabled(insuranceTabIndex, peristance);
      ui_.tabWidget->setTabEnabled(paysheetTabIndex, peristance);
      ui_.tabWidget->setTabEnabled(sbolTabIndex, peristance);
      ui_.tabWidget->setTabEnabled(bolTabIndex, peristance);

      relatedDocsAction_->setEnabled(peristance);

      ActionUtility::enable(contractCcdListPage_->parentActions(), peristance);                  
      ActionUtility::enable(contractCcdListPage_->selectedParentActions(), 
        peristance && ccdTabIndex == currentTabIndex && contractCcdListPage_->selectedId());            

      ActionUtility::enable(contractInsuranceListPage_->parentActions(), peristance);            
      ActionUtility::enable(contractInsuranceListPage_->selectedParentActions(), 
        peristance && insuranceTabIndex == currentTabIndex && contractInsuranceListPage_->selectedId());      
      
      ActionUtility::enable(contractPaysheetListPage_->parentActions(), peristance);                  
      ActionUtility::enable(contractPaysheetListPage_->selectedParentActions(), 
        peristance && paysheetTabIndex == currentTabIndex && contractPaysheetListPage_->selectedId());            

      ActionUtility::enable(contractSbolListPage_->parentActions(), peristance);            
      ActionUtility::enable(contractSbolListPage_->selectedParentActions(), 
        peristance && sbolTabIndex == currentTabIndex && contractSbolListPage_->selectedId());            

      ActionUtility::enable(contractBolListPage_->parentActions(), peristance);            
      ActionUtility::enable(contractBolListPage_->selectedParentActions(), 
        peristance && bolTabIndex == currentTabIndex && contractBolListPage_->selectedId());            

      ui_.closedCheckBox->setEnabled(peristance && !readOnly);
      ui_.closeUserBtn->setEnabled(entityData_.closeUserId);      

      switch (currentMode)
      {
      case viewMode:
        if (entityId())
        {
          updateWindowTitle(tr("Contracts - %1").arg(entityData_.number));
        }
        showRequiredMarkers(false);
        break;

      case editMode:
        if (entityId())
        {
          if (contentsChanged_)
          {
            updateWindowTitle(tr("Contracts - %1* - edit").arg(entityData_.number));
          }
          else
          {
            updateWindowTitle(tr("Contracts - %1 - edit").arg(entityData_.number));
          }        
        }
        showRequiredMarkers(true);
        break;

      case createMode:
        updateWindowTitle(tr("Contracts - New contract creation*"));
        showRequiredMarkers(true);
        break;
      }      
    }

    bool ContractCardPage::populateFromWidgets()
    {
      static const char* contractNumberTitle = QT_TR_NOOP("Contract number");      
      static const char* passportNumberTitle = QT_TR_NOOP("Passport number");      
      static const char* amountTitle = QT_TR_NOOP("Amount");
      static const char* currencyTitle = QT_TR_NOOP("Currency");
      static const char* counterpartTitle = QT_TR_NOOP("Counterpart");
      static const char* bankTitle = QT_TR_NOOP("Bank");
      static const char* residentTitle = QT_TR_NOOP("Resident");
      static const char* remarkTitle = QT_TR_NOOP("Remark");
      static const char* closeRemarkTitle = QT_TR_NOOP("Close remarks");
      
      static const char* messageTitle = QT_TR_NOOP("Invalid data input");
      static const char* notEmpty = QT_TR_NOOP("Field \"%1\" can\'t be empty.");
      static const char* tooLongText = QT_TR_NOOP("Field \"%1\" can\'t exceed %2 characters.");      
      static const char* invalidFieldValueText = QT_TR_NOOP("Invalid value in field \"%1\".");      

      int contentsTabIndex = ui_.tabWidget->indexOf(ui_.contentsTab);
      int remarksTabIndex = ui_.tabWidget->indexOf(ui_.remarksTab);
      int closeTabIndex = ui_.tabWidget->indexOf(ui_.closeTab);
      bool validated;
      // Get contract number
      QString number = ui_.contractNumberEdit->text().trimmed();
      if (number.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(contractNumberTitle)));
      } 
      else if (number.length() > model::Contract::contractNumberMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(contractNumberTitle)).arg(model::Contract::contractNumberMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.contractNumberEdit->setFocus(Qt::TabFocusReason);
        ui_.contractNumberEdit->selectAll();
        return false;
      } 
      // Get passport number
      QString passportNumber = ui_.passportNumberEdit->text().trimmed();
      if (passportNumber.length() > model::Contract::passportNumberMaxLen)
      {        
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(passportNumberTitle)).arg(model::Contract::passportNumberMaxLen));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.passportNumberEdit->setFocus(Qt::TabFocusReason);
        ui_.passportNumberEdit->selectAll();
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
      if (!currencyAmount.setValue(ui_.amountEdit->text().trimmed()))
      {
        QMessageBox::warning(this, tr(messageTitle), tr(invalidFieldValueText).arg(tr(amountTitle)));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.amountEdit->setFocus(Qt::TabFocusReason);
        ui_.amountEdit->selectAll();
        return false;
      }
      // Get remark
      QString remark = ui_.remarksEdit->toPlainText().trimmed();
      if (remark.length() > model::Contract::remarkMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(remarkTitle)).arg(model::Contract::remarkMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(remarksTabIndex);
        ui_.remarksEdit->setFocus(Qt::TabFocusReason);
        ui_.remarksEdit->selectAll();
        return false;
      }            
      if (!selectedCounterpartId_)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(counterpartTitle)));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.counterpartBtn->setFocus(Qt::TabFocusReason);
        return false;
      }
      if (!selectedBankId_)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(bankTitle)));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.bankBtn->setFocus(Qt::TabFocusReason);
        return false;
      }
      if (!selectedResidentId_)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(residentTitle)));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.residentBtn->setFocus(Qt::TabFocusReason);
        return false;
      }
      // Get close remark
      QString closeRemark = ui_.closeRemarksEdit->toPlainText().trimmed();
      if (closeRemark.length() > model::Contract::closeRemarkMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(closeRemarkTitle)).arg(model::Contract::closeRemarkMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(closeTabIndex);
        ui_.closeRemarksEdit->setFocus(Qt::TabFocusReason);
        ui_.closeRemarksEdit->selectAll();
        return false;
      }
      // Get contract close data
      bool closeMark = Qt::Checked == ui_.closedCheckBox->checkState();
      OptionalQDate closeDate = closeMark ? ui_.closeDateEdit->date() : OptionalQDate();

      // Get difference between old and new data
      if (!entityData_.closeMark && closeMark)
      {
        closeDataUpdated_ = true;
      }
      else if (entityData_.closeMark && !closeMark)
      {
        closeDataUpdated_ = true;
      }
      else if (entityData_.closeMark && closeMark)
      {
        closeDataUpdated_ = closeDate.get() != entityData_.closeDate.get();
      }

      entityData_.residentId = selectedResidentId_.get();
      entityData_.currencyId = selectedCurrencyId_.get();
      entityData_.counterpartId = selectedCounterpartId_.get();
      entityData_.bankId = selectedBankId_.get();
      entityData_.number = number;      
      entityData_.amount = currencyAmount.amount();
      entityData_.passportNumber = passportNumber.isEmpty() ? OptionalQString() : passportNumber;
      entityData_.signingDate = ui_.signingDateEdit->date();
      entityData_.endDate = ui_.endDateEdit->date();      
      entityData_.closeMark = closeMark;
      entityData_.closeRemark = closeRemark.isEmpty() ? OptionalQString() : closeRemark;
      entityData_.remark = remark.isEmpty() ? OptionalQString() : remark;
      entityData_.passportSigningDate = ui_.passportSigningDateEdit->date();
      entityData_.passportEndDate = ui_.passportEndDateEdit->date();
      entityData_.closeDate = closeDate;
      return true;
    }

    bool ContractCardPage::populateFromDatabase()
    {      
      static const char* querySql = "select " \
        "t.\"NUMBER\", t.\"AMOUNT\", "
        "t.\"PASSPORT_NUMBER\", t.\"CLOSE_MARK\", " \
        "t.\"SIGNING_DATE\", t.\"END_DATE\", " \
        "t.\"REMARK\", t.\"CLOSE_REMARK\", " \
        "t.\"RESIDENT_ID\", r.\"SHORT_NAME\", " \
        "t.\"CURRENCY_ID\", cur.\"FULL_NAME\", " \
        "t.\"COUNTERPART_ID\", cntp.\"SHORT_NAME\", " \
        "t.\"BANK_ID\", b.\"SHORT_NAME\", " \
        "t.\"CREATE_USER_ID\", cu.\"LOGIN\", " \
        "t.\"UPDATE_USER_ID\", uu.\"LOGIN\", " \
        "t.\"CLOSE_USER_ID\", clu.\"LOGIN\", " \
        "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", t.\"CLOSE_TIME\", " \
        "t.\"PASSPORT_END_DATE\", t.\"CLOSE_DATE\", t.\"PASSPORT_SIGNING_DATE\" " \
        "from \"CONTRACT\" t " \
        "join \"RESIDENT\" r on t.\"RESIDENT_ID\" = r.\"RESIDENT_ID\""
        "join \"CURRENCY\" cur on t.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
        "join \"COUNTERPART\" cntp on t.\"COUNTERPART_ID\" = cntp.\"COUNTERPART_ID\" " \
        "join \"BANK\" b on t.\"BANK_ID\" = b.\"BANK_ID\" " \
        "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
        "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
        "left join \"USER\" clu on t.\"CLOSE_USER_ID\" = clu.\"USER_ID\" " \
        "where t.\"CONTRACT_ID\" = ?";

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
              entityData_.contractId = entityId().get();
              entityData_.number     = query.value(0).toString();
              entityData_.amount     = query.value(1).toLongLong();
              entityData_.passportNumber = query.isNull(2) ? OptionalQString() : query.value(2).toString(); 
              entityData_.closeMark  = query.value(3).toInt() != 0;
              entityData_.signingDate = databaseModel()->convertFromServer(query.value(4).toDate());
              entityData_.endDate     = databaseModel()->convertFromServer(query.value(5).toDate());
              entityData_.remark      = query.isNull(6) ? OptionalQString() : query.value(6).toString();
              entityData_.closeRemark = query.isNull(7) ? OptionalQString() : query.value(7).toString();          
              entityData_.residentId  = query.value(8).toLongLong();
              residentShortName_      = query.value(9).toString();
              entityData_.currencyId  = query.value(10).toLongLong();
              currencyFullName_       = query.value(11).toString();
              entityData_.counterpartId = query.value(12).toLongLong();
              counterpartShortName_   = query.value(13).toString();
              entityData_.bankId      = query.value(14).toLongLong();
              bankShortName_          = query.value(15).toString();
              entityData_.createUserId = query.value(16).toLongLong();
              createUserName_          = query.value(17).toString(); 
              entityData_.updateUserId = query.value(18).toLongLong();
              updateUserName_          = query.value(19).toString(); 
              entityData_.closeUserId  = query.isNull(20) ? OptionalQInt64()  : query.value(20).toLongLong();
              closeUserName_           = query.isNull(21) ? OptionalQString() : query.value(21).toString(); 
              entityData_.createTime   = databaseModel()->convertFromServer(query.value(22).toDateTime()); 
              entityData_.updateTime   = databaseModel()->convertFromServer(query.value(23).toDateTime()); 
              entityData_.closeTime    = query.isNull(24) ? OptionalQDateTime() : databaseModel()->convertFromServer(query.value(24).toDateTime());              
              entityData_.passportEndDate = databaseModel()->convertFromServer(query.value(25).toDate()); 
              entityData_.closeDate    = query.isNull(26) ? OptionalQDate() : databaseModel()->convertFromServer(query.value(26).toDate()); 
              entityData_.passportSigningDate = databaseModel()->convertFromServer(query.value(27).toDate()); 
              closeDataUpdated_ = false;
              return true;
            }
          }
        }
      }
      return false;
    }

    void ContractCardPage::populateWidgets()
    {
      if (entityId())
      {
        // Contents page
        ui_.contractNumberEdit->setText(entityData_.number);
        ui_.passportNumberEdit->setText(entityData_.passportNumber ? entityData_.passportNumber.get() : QString());
        ui_.closedCheckBox->setCheckState(entityData_.closeMark ? Qt::Checked : Qt::Unchecked);
        ui_.signingDateEdit->setDate(entityData_.signingDate);
        ui_.endDateEdit->setDate(entityData_.endDate);
        ui_.amountEdit->setText(model::CurrencyAmount(entityData_.amount).toString());               
        ui_.remarksEdit->setPlainText(entityData_.remark ? entityData_.remark.get() : QString());
        ui_.closeRemarksEdit->setPlainText(entityData_.closeRemark ? entityData_.closeRemark.get() : QString());
        ui_.currencyEdit->setText(currencyFullName_);
        ui_.bankEdit->setText(bankShortName_);
        ui_.counterpartEdit->setText(counterpartShortName_);
        ui_.residentEdit->setText(residentShortName_);
        selectedCurrencyId_    = entityData_.currencyId;
        selectedBankId_        = entityData_.bankId;
        selectedCounterpartId_ = entityData_.counterpartId;
        selectedResidentId_    = entityData_.residentId;        
        ui_.passportSigningDateEdit->setDate(entityData_.passportSigningDate);
        ui_.passportEndDateEdit->setDate(entityData_.passportEndDate);
        if (entityData_.closeMark)
        {
          ui_.closeStackedWidget->setCurrentIndex(1);
          ui_.closeDateEdit->setDate(entityData_.closeDate.get());
        }
        else
        {
          ui_.closeStackedWidget->setCurrentIndex(0);
        }
        // General data page
        ui_.createTimeEdit->setText(entityData_.createTime.toString(Qt::DefaultLocaleShortDate));
        ui_.updateTimeEdit->setText(entityData_.updateTime.toString(Qt::DefaultLocaleShortDate));
        ui_.createUserEdit->setText(createUserName_);
        ui_.updateUserEdit->setText(updateUserName_);
        // Close page
        ui_.closeTimeEdit->setText(entityData_.closeTime ? entityData_.closeTime.get().toString(Qt::DefaultLocaleShortDate) :  QString());
        ui_.closeUserEdit->setText(closeUserName_ ? closeUserName_.get() : QString());
        populateExportData();
        contentsChanged_ = false;
      }
    }    

    void ContractCardPage::populateExportData()
    {
      static const QString contractNumberCaption = tr("Contract number");
      static const QString signDateCaption = tr("Contract signinig date");      
      static const QString endDateCaption = tr("Contract execution end date");
      static const QString passportNumberCaption = tr("Transaction passport number");
      static const QString passportSignDateCaption = tr("Passport signinig date");
      static const QString passportEndDateCaption = tr("Passport end date");
      static const QString closeMarkCaption = tr("Close mark");
      static const QString closeDateCaption = tr("Close date");
      static const QString amountCaption = tr("Amount");
      static const QString currencyCaption = tr("Currency");
      static const QString counterpartCaption = tr("Counterpart");
      static const QString bankCaption = tr("Bank");
      static const QString residentCaption = tr("Resident");
      static const QString remarksCaption = tr("Remarks");
      static const QString closeUserCaption = tr("User, closed");
      static const QString closeTimeCaption = tr("Time, closed");
      static const QString closeRemarksCaption = tr("Close remarks");
      static const QString createUserCaption = tr("User, creator");
      static const QString createTimeCaption = tr("Time, created");
      static const QString updateUserCaption = tr("User, last update");
      static const QString updateTimeCaption = tr("Time, last update");
        
      ExportDataList exportDataList;
      exportDataList << std::make_pair(contractNumberCaption, QVariant(entityData_.number));
      exportDataList << std::make_pair(signDateCaption, QVariant(entityData_.signingDate));
      exportDataList << std::make_pair(endDateCaption, QVariant(entityData_.endDate));
      exportDataList << std::make_pair(passportNumberCaption, entityData_.passportNumber ? QVariant(entityData_.passportNumber.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(passportSignDateCaption, QVariant(entityData_.passportSigningDate));
      exportDataList << std::make_pair(passportEndDateCaption, QVariant(entityData_.passportEndDate));
      exportDataList << std::make_pair(closeMarkCaption, QVariant(entityData_.closeMark));
      exportDataList << std::make_pair(closeDateCaption, entityData_.closeDate ? QVariant(entityData_.closeDate.get()) : QVariant(QVariant::Date));
      exportDataList << std::make_pair(amountCaption, QVariant::fromValue(model::CurrencyAmount(entityData_.amount)));
      exportDataList << std::make_pair(currencyCaption, QVariant(currencyFullName_));
      exportDataList << std::make_pair(counterpartCaption, QVariant(counterpartShortName_));
      exportDataList << std::make_pair(bankCaption, QVariant(bankShortName_));
      exportDataList << std::make_pair(residentCaption, QVariant(residentShortName_));
      exportDataList << std::make_pair(remarksCaption, entityData_.remark ? QVariant(entityData_.remark.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(closeUserCaption, closeUserName_ ? QVariant(closeUserName_.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(closeTimeCaption, entityData_.closeTime ? QVariant(entityData_.closeTime.get()) : QVariant(QVariant::DateTime));
      exportDataList << std::make_pair(closeRemarksCaption, entityData_.closeRemark ? QVariant(entityData_.closeRemark.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(createUserCaption, QVariant(createUserName_));
      exportDataList << std::make_pair(createTimeCaption, QVariant(entityData_.createTime));
      exportDataList << std::make_pair(updateUserCaption, QVariant(updateUserName_));
      exportDataList << std::make_pair(updateTimeCaption, QVariant(entityData_.updateTime));      
            
      setExportDataList(exportDataList);
    }

    bool ContractCardPage::updateDatabase()
    {      
      static const char* querySql = "update \"CONTRACT\" set " \
        "\"RESIDENT_ID\" = ?, \"CURRENCY_ID\" = ?, \"REMARK\" = ?, " \
        "\"COUNTERPART_ID\" = ?, \"NUMBER\" = ?, " \
        "\"BANK_ID\" = ?, \"AMOUNT\" = ?, \"PASSPORT_NUMBER\" = ?, " \
        "\"CLOSE_USER_ID\" = ?, \"CLOSE_MARK\" = 1, \"SIGNING_DATE\" = ?, " \
        "\"END_DATE\" = ?, \"CLOSE_REMARK\" = ?, \"CLOSE_TIME\" = 'NOW', " \
        "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\', " \
        "\"PASSPORT_END_DATE\" = ?, \"CLOSE_DATE\" = ?, \"PASSPORT_SIGNING_DATE\" = ? " \
        "where \"CONTRACT_ID\" = ?";

      static const char* querySql2 = "update \"CONTRACT\" set " \
        "\"RESIDENT_ID\" = ?, \"CURRENCY_ID\" = ?, \"REMARK\" = ?, " \
        "\"COUNTERPART_ID\" = ?, \"NUMBER\" = ?, " \
        "\"BANK_ID\" = ?, \"AMOUNT\" = ?, \"PASSPORT_NUMBER\" = ?, " \
        "\"CLOSE_USER_ID\" = NULL, \"CLOSE_MARK\" = 0, \"SIGNING_DATE\" = ?, " \
        "\"END_DATE\" = ?, \"CLOSE_REMARK\" = NULL, \"CLOSE_TIME\" = NULL, " \
        "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\', " \
        "\"PASSPORT_END_DATE\" = ?, \"CLOSE_DATE\" = NULL, \"PASSPORT_SIGNING_DATE\" = ? " \
        "where \"CONTRACT_ID\" = ?";

      static const char* querySql3 = "update \"CONTRACT\" set " \
        "\"RESIDENT_ID\" = ?, \"CURRENCY_ID\" = ?, \"REMARK\" = ?, " \
        "\"COUNTERPART_ID\" = ?, \"NUMBER\" = ?, " \
        "\"BANK_ID\" = ?, \"AMOUNT\" = ?, \"PASSPORT_NUMBER\" = ?, " \
        "\"SIGNING_DATE\" = ?, \"END_DATE\" = ?, \"CLOSE_REMARK\" = ?, " \
        "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\', " \
        "\"PASSPORT_END_DATE\" = ?, \"PASSPORT_SIGNING_DATE\" = ? " \
        "where \"CONTRACT_ID\" = ?";

      if (entityId())
      {
        bool queryReady = false;
        QSqlQuery query(databaseModel()->database());   
        if (closeDataUpdated_)
        {
          if (entityData_.closeMark)
          {
            if (query.prepare(querySql))
            {
              query.bindValue(0, entityData_.residentId);
              query.bindValue(1, entityData_.currencyId);
              query.bindValue(2, entityData_.remark ? entityData_.remark.get() : QVariant(QVariant::String));
              query.bindValue(3, entityData_.counterpartId);
              query.bindValue(4, entityData_.number);
              query.bindValue(5, entityData_.bankId);
              query.bindValue(6, entityData_.amount);
              query.bindValue(7, entityData_.passportNumber ? entityData_.passportNumber.get() : QVariant(QVariant::String));
              query.bindValue(8, databaseModel()->userId());          
              query.bindValue(9, databaseModel()->convertToServer(entityData_.signingDate));
              query.bindValue(10, databaseModel()->convertToServer(entityData_.endDate));
              query.bindValue(11, entityData_.closeRemark ? entityData_.closeRemark.get() : QVariant(QVariant::String));
              query.bindValue(12, databaseModel()->userId());              
              query.bindValue(13, databaseModel()->convertToServer(entityData_.passportEndDate));
              query.bindValue(14, databaseModel()->convertToServer(entityData_.closeDate.get()));
              query.bindValue(15, databaseModel()->convertToServer(entityData_.passportSigningDate));
              query.bindValue(16, entityData_.contractId);          
              queryReady = true;
            }
          }
          else
          {
            if (query.prepare(querySql2))
            {
              query.bindValue(0, entityData_.residentId);
              query.bindValue(1, entityData_.currencyId);
              query.bindValue(2, entityData_.remark ? entityData_.remark.get() : QVariant(QVariant::String));
              query.bindValue(3, entityData_.counterpartId);
              query.bindValue(4, entityData_.number);
              query.bindValue(5, entityData_.bankId);
              query.bindValue(6, entityData_.amount);
              query.bindValue(7, entityData_.passportNumber ? entityData_.passportNumber.get() : QVariant(QVariant::String));
              query.bindValue(8, databaseModel()->convertToServer(entityData_.signingDate));
              query.bindValue(9, databaseModel()->convertToServer(entityData_.endDate));
              query.bindValue(10, databaseModel()->userId()); 
              query.bindValue(11, databaseModel()->convertToServer(entityData_.passportEndDate));
              query.bindValue(12, databaseModel()->convertToServer(entityData_.passportSigningDate));
              query.bindValue(13, entityData_.contractId);          
              queryReady = true;
            }
          }
        }
        else 
        {
          if (query.prepare(querySql3))
          {
            query.bindValue(0, entityData_.residentId);
            query.bindValue(1, entityData_.currencyId);
            query.bindValue(2, entityData_.remark ? entityData_.remark.get() : QVariant(QVariant::String));
            query.bindValue(3, entityData_.counterpartId);
            query.bindValue(4, entityData_.number);
            query.bindValue(5, entityData_.bankId);
            query.bindValue(6, entityData_.amount);
            query.bindValue(7, entityData_.passportNumber ? entityData_.passportNumber.get() : QVariant(QVariant::String));              
            query.bindValue(8, databaseModel()->convertToServer(entityData_.signingDate));
            query.bindValue(9, databaseModel()->convertToServer(entityData_.endDate));
            query.bindValue(10, entityData_.closeRemark ? entityData_.closeRemark.get() : QVariant(QVariant::String));
            query.bindValue(11, databaseModel()->userId());              
            query.bindValue(12, databaseModel()->convertToServer(entityData_.passportEndDate));              
            query.bindValue(13, databaseModel()->convertToServer(entityData_.passportSigningDate));
            query.bindValue(14, entityData_.contractId);          
            queryReady = true;
          }
        }
        if (queryReady)
        {
          if (query.exec())
          {
            if (0 < query.numRowsAffected())
            {
              databaseModel()->notifyUpdate(entityData_);
              closeDataUpdated_ = false;
              return true;
            }            
          }
        }
      }
      return false;
    }

    bool ContractCardPage::insertIntoDatabase()
    {
#ifdef MA_CHRONO_BUILD_ENVIRONMENT_DEMO      
      return false;
#else
      static const char* querySql = "insert into \"CONTRACT\"(" \
        "\"CONTRACT_ID\", \"RESIDENT_ID\", \"CURRENCY_ID\", " \
        "\"COUNTERPART_ID\", \"NUMBER\", \"CREATE_USER_ID\", \"BANK_ID\", " \
        "\"AMOUNT\", \"PASSPORT_NUMBER\", \"UPDATE_USER_ID\", \"CLOSE_MARK\", " \
        "\"SIGNING_DATE\", \"END_DATE\", \"REMARK\", \"CREATE_TIME\", \"UPDATE_TIME\", " \
        "\"PASSPORT_END_DATE\", \"PASSPORT_SIGNING_DATE\") " \
        "values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 0, ?, ?, ?, \'NOW\', \'NOW\', ?, ?)";
      
      if (OptionalQInt64 newEntityId = databaseModel()->generateId(contractEntity))
      {
        entityData_.contractId = newEntityId.get();
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.contractId);
          query.bindValue(1, entityData_.residentId);
          query.bindValue(2, entityData_.currencyId);
          query.bindValue(3, entityData_.counterpartId);
          query.bindValue(4, entityData_.number);          
          query.bindValue(5, databaseModel()->userId());
          query.bindValue(6, entityData_.bankId);     
          query.bindValue(7, entityData_.amount);     
          query.bindValue(8, entityData_.passportNumber ? entityData_.passportNumber.get() : QVariant(QVariant::String));
          query.bindValue(9, databaseModel()->userId());          
          query.bindValue(10, databaseModel()->convertToServer(entityData_.signingDate));
          query.bindValue(11, databaseModel()->convertToServer(entityData_.endDate));          
          query.bindValue(12, entityData_.remark ? entityData_.remark.get() : QVariant(QVariant::String));          
          query.bindValue(13, databaseModel()->convertToServer(entityData_.passportEndDate));          
          query.bindValue(14, databaseModel()->convertToServer(entityData_.passportSigningDate));          
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
#endif // MA_CHRONO_BUILD_ENVIRONMENT_DEMO      
    }

    bool ContractCardPage::removeFromDatabase()
    {
      static const char* querySql = "delete from \"CONTRACT\" where \"CONTRACT_ID\" = ?";

      if (QMessageBox::Yes == QMessageBox::question(this, tr("Contracts"), 
        tr("Delete contract?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
      {
        if (entityId())
        {
          QSqlQuery query(databaseModel()->database());              
          if (query.prepare(querySql))
          {
            query.bindValue(0, entityData_.contractId);
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
    
    void ContractCardPage::showRequiredMarkers(bool visible)
    {
      WidgetUtility::show(requiredMarkers_, visible);
    }

    void ContractCardPage::on_tabWidget_currentChanged(int /*index*/)
    {
      updateWidgets();      
    }

    void ContractCardPage::on_ccd_doubleClicked(const QModelIndex& /*index*/)
    {
      contractCcdListPage_->viewSelected();
    }

    void ContractCardPage::on_insurance_doubleClicked(const QModelIndex& /*index*/)
    {
      contractInsuranceListPage_->viewSelected();
    }

    void ContractCardPage::on_paysheet_doubleClicked(const QModelIndex& /*index*/)
    {
      contractPaysheetListPage_->viewSelected();
    }

    void ContractCardPage::on_sbol_doubleClicked(const QModelIndex& /*index*/)
    {
      contractSbolListPage_->viewSelected();
    }

    void ContractCardPage::on_bol_doubleClicked(const QModelIndex& /*index*/)
    {
      contractBolListPage_->viewSelected();
    }

    void ContractCardPage::on_lineEdit_textEdited(const QString&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void ContractCardPage::on_textEdit_textChanged()
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void ContractCardPage::on_dateEdit_dateChanged(const QDate&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void ContractCardPage::on_checkBox_stateChanged(int)
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void ContractCardPage::on_closedCheckBox_stateChanged(int state)
    {
      if (Qt::Checked == state)
      {
        ui_.closeStackedWidget->setCurrentIndex(1);
      }
      else 
      {
        ui_.closeStackedWidget->setCurrentIndex(0);
      }
    }

    void ContractCardPage::on_database_userUpdated(const ma::chrono::model::User& user)
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
        if (entityData_.closeUserId)
        {        
          if (entityData_.closeUserId.get() == user.userId)
          {
            closeUserName_ = user.userName;
            ui_.closeUserEdit->setText(closeUserName_.get());
            updated = true;
          }
        }
        if (updated)
        {          
          updateWidgets();
        }
      }
    }

    void ContractCardPage::on_database_currencyUpdated(const ma::chrono::model::Currency& currency)
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

    void ContractCardPage::on_database_currencyRemoved(const ma::chrono::model::Currency& currency)
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

    void ContractCardPage::on_database_counterpartUpdated(const ma::chrono::model::Counterpart& counterpart)
    {
      if (entityId())
      {        
        if (entityData_.counterpartId == counterpart.counterpartId)
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

    void ContractCardPage::on_database_counterpartRemoved(const ma::chrono::model::Counterpart& counterpart)
    {
      if (selectedCounterpartId_)
      {
        if (selectedCounterpartId_.get() == counterpart.counterpartId)
        {          
          ui_.counterpartEdit->setText(QString());
          selectedCounterpartId_.reset();
          updateWidgets();
        }
      }
    }

    void ContractCardPage::on_database_bankUpdated(const ma::chrono::model::Bank& bank)
    {
      if (entityId())
      {        
        if (entityData_.bankId == bank.bankId)
        {
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

    void ContractCardPage::on_database_bankRemoved(const ma::chrono::model::Bank& bank)
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

    void ContractCardPage::on_database_residentUpdated(const ma::chrono::model::Resident& resident)
    {
      if (entityId())
      {        
        if (entityData_.residentId == resident.residentId)
        {
          residentShortName_ = resident.shortName;
        }        
      }
      if (selectedResidentId_)
      {
        if (selectedResidentId_.get() == resident.residentId)
        {          
          ui_.residentEdit->setText(resident.shortName);
          updateWidgets();
        }
      }
    }

    void ContractCardPage::on_database_residentRemoved(const ma::chrono::model::Resident& resident)
    {
      if (selectedResidentId_)
      {
        if (selectedResidentId_.get() == resident.residentId)
        {          
          ui_.residentEdit->setText(QString());
          selectedResidentId_.reset();
          updateWidgets();
        }
      }
    }

    void ContractCardPage::on_currencyBtn_clicked(bool /*checked*/)
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

    void ContractCardPage::on_counterpartBtn_clicked(bool /*checked*/)
    {      
      if (viewMode == mode())
      {
        if (entityId())
        {
          showCounterpartCardTaskWindow(entityData_.counterpartId);
        }        
      }
      else
      {
        //todo: add default selection
        std::auto_ptr<ListPage> listPage(new CounterpartListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), 
          UserDefinedFilter(), UserDefinedOrder(), 0));
        ListSelectDialog listSelectDialog(this, listPage);
        listSelectDialog.setWindowModality(Qt::WindowModal);
        if (QDialog::Accepted == listSelectDialog.exec())
        {
          selectedCounterpartId_ = listSelectDialog.listPage()->selectedId();          
          if (OptionalQString counterpartShortName = listSelectDialog.listPage()->selectedRelationalText())
          {
            ui_.counterpartEdit->setText(counterpartShortName.get());
          }
          contentsChanged_ = true;
          updateWidgets();
        }
      }      
    }

    void ContractCardPage::on_bankBtn_clicked(bool /*checked*/)
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
          //todo bank code from database - not from model
          if (BankListPage* bankListPage = qobject_cast<BankListPage*>(listSelectDialog.listPage()))
          {
            if (OptionalInt bankCode = bankListPage->selectedBankCode())
            {              
              QString oldPassportNumber = ui_.passportNumberEdit->displayText();
              int prefixEnd = oldPassportNumber.indexOf("/");
              if (-1 != prefixEnd)
              {
                int suffixStart = oldPassportNumber.indexOf("/", prefixEnd + 1);
                if (-1 != suffixStart)
                {                  
                  QString prefix = oldPassportNumber.left(prefixEnd + 1);
                  QString suffix = oldPassportNumber.mid(suffixStart, oldPassportNumber.length() - suffixStart);
                  QString bankText = QString("%1").arg(bankCode.get(), 4, 10, QLatin1Char('0'));
                  ui_.passportNumberEdit->setText(prefix + bankText + suffix);
                }
              }                            
            }
          }
          contentsChanged_ = true;
          updateWidgets();
        }
      }      
    }

    void ContractCardPage::on_residentBtn_clicked(bool /*checked*/)
    {      
      if (viewMode == mode())
      {
        if (entityId())
        {          
          showResidentCardTaskWindow(entityData_.residentId);
        }        
      }
      else
      {
        //todo: add default selection
        std::auto_ptr<ListPage> listPage(new ResidentListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), 
          UserDefinedFilter(), UserDefinedOrder(), 0));
        ListSelectDialog listSelectDialog(this, listPage);
        listSelectDialog.setWindowModality(Qt::WindowModal);
        if (QDialog::Accepted == listSelectDialog.exec())
        {          
          selectedResidentId_ = listSelectDialog.listPage()->selectedId();          
          if (OptionalQString residentName = listSelectDialog.listPage()->selectedRelationalText())
          {
            ui_.residentEdit->setText(residentName.get());
          }
          contentsChanged_ = true;
          updateWidgets();
        }
      }      
    }

    void ContractCardPage::on_createUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.createUserId);
      }        
    }

    void ContractCardPage::on_updateUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.updateUserId);
      }        
    }

    void ContractCardPage::on_closeUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {
        if (entityData_.closeUserId)
        {
          showUserCardTaskWindow(entityData_.closeUserId.get());
        }        
      }        
    }        

  } // namespace chrono
} // namespace ma
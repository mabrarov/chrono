/*
TRANSLATOR ma::chrono::CcdCardPage
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
#include <ma/chrono/currencylistpage.h>
#include <ma/chrono/contractlistpage.h>
#include <ma/chrono/counterpartlistpage.h>
#include <ma/chrono/banklistpage.h>
#include <ma/chrono/ttermlistpage.h>
#include <ma/chrono/deppointlistpage.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/widgetutility.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/counterpart.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/autoenterprise.h>
#include <ma/chrono/model/deppoint.h>
#include <ma/chrono/model/bank.h>
#include <ma/chrono/model/tterm.h>
#include <ma/chrono/model/currencyamount.h>
#include <ma/chrono/model/currency.h>
#include <ma/chrono/model/rateamount.h>
#include <ma/chrono/localeutility.h>
#include <ma/chrono/ccdcardpage.h>

namespace ma
{
  namespace chrono
  {    
    CcdCardPage::CcdCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent)
      : CardPage(resourceManager, taskWindowManager, databaseModel, helpAction, ccdEntity, mode, entityId, parent)
      , dataAwareWidgets_()
      , contentsChanged_(false)      
    {
      ui_.setupUi(this);          
      ui_.tabWidget->setCurrentIndex(0);
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      LocaleUtility::setupLocaleAwareWidget(ui_.exportPermDate);
      LocaleUtility::setupLocaleAwareWidget(ui_.exportDate);      
      LocaleUtility::setupLocaleAwareWidget(ui_.paymentTermDate);
      ui_.paymentTermTypeCombo->setCurrentIndex(0);
      ui_.paymentTermStackedWidget->setCurrentIndex(0);
      ui_.paymentTermPeriodSpin->setValue(50);
      ui_.paymentTermPeriodSpin->setMinimum(model::Ccd::paymentTermPeriodMin);
      ui_.paymentTermPeriodSpin->setMaximum(model::Ccd::paymentTermPeriodMax);

      createAction()->setText(tr("&New CCD"));
      dataAwareWidgets_ << ui_.ccdNumberEdit << ui_.amountEdit << ui_.exportPermDate 
                        << ui_.exportCheck << ui_.exportDate << ui_.productCodeEdit
                        << ui_.dutyEdit << ui_.seriesTextEdit << ui_.rateEdit
                        << ui_.zeroTaxPeriodCombo << ui_.paymentControlCheck << ui_.zeroTaxControlCheck
                        << ui_.paymentTermTypeCombo << ui_.paymentTermDate << ui_.paymentTermPeriodSpin;
      requiredMarkers_  << ui_.asterikLabel << ui_.asterikLabel_2 << ui_.asterikLabel_3 
                        << ui_.asterikLabel_4 << ui_.asterikLabel_5 << ui_.asterikLabel_6;
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
      QObject::connect(databaseModel.get(), SIGNAL(counterpartUpdated(const ma::chrono::model::Counterpart&)), SLOT(on_database_counterpartUpdated(const ma::chrono::model::Counterpart&)));            
      QObject::connect(databaseModel.get(), SIGNAL(currencyUpdated(const ma::chrono::model::Currency&)), SLOT(on_database_currencyUpdated(const ma::chrono::model::Currency&)));
      QObject::connect(databaseModel.get(), SIGNAL(bankUpdated(const ma::chrono::model::Bank&)), SLOT(on_database_bankUpdated(const ma::chrono::model::Bank&)));            
      QObject::connect(databaseModel.get(), SIGNAL(contractUpdated(const ma::chrono::model::Contract&)), SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));
      QObject::connect(databaseModel.get(), SIGNAL(contractRemoved(const ma::chrono::model::Contract&)), SLOT(on_database_contractRemoved(const ma::chrono::model::Contract&)));
      QObject::connect(databaseModel.get(), SIGNAL(deppointUpdated(const ma::chrono::model::Deppoint&)), SLOT(on_database_deppointUpdated(const ma::chrono::model::Deppoint&)));
      QObject::connect(databaseModel.get(), SIGNAL(deppointRemoved(const ma::chrono::model::Deppoint&)), SLOT(on_database_deppointRemoved(const ma::chrono::model::Deppoint&)));      
      QObject::connect(databaseModel.get(), SIGNAL(ttermUpdated(const ma::chrono::model::Tterm&)), SLOT(on_database_ttermUpdated(const ma::chrono::model::Tterm&)));
      QObject::connect(databaseModel.get(), SIGNAL(ttermRemoved(const ma::chrono::model::Tterm&)), SLOT(on_database_ttermRemoved(const ma::chrono::model::Tterm&)));
      updateWidgets();      
    }

    CcdCardPage::~CcdCardPage()
    {
    } 

    void CcdCardPage::refresh()
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

    void CcdCardPage::save()
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

    void CcdCardPage::cancel()
    {
      if (editMode == mode())
      {      
        populateWidgets();
      }      
      CardPage::cancel();
      updateWidgets();
    }

    void CcdCardPage::edit()
    {      
      CardPage::edit();
      updateWidgets();
    }

    void CcdCardPage::remove()
    {
      if (createMode != mode())
      {
        if (removeFromDatabase())
        {
          CardPage::remove();
        }
      }            
    }

    std::auto_ptr<CardPage> CcdCardPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new CcdCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }

    void CcdCardPage::setSelectedContractId(qint64 contractId)
    {
      selectedContractId_ = contractId;            
      updateContractRelatedNonPersistData(contractId);
    }

    void CcdCardPage::connectDataAwareWidgets()
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
        else if (QSpinBox* edit = qobject_cast<QSpinBox*>(widget))
        {
          QObject::connect(edit, SIGNAL(valueChanged(const QString&)), SLOT(on_lineEdit_textEdited(const QString&)));
        }        
        else if (QDateEdit* edit = qobject_cast<QDateEdit*>(widget))
        {
          QObject::connect(edit, SIGNAL(dateChanged(const QDate&)), SLOT(on_dateEdit_dateChanged(const QDate&)));
        }
        else if (QCheckBox* edit = qobject_cast<QCheckBox*>(widget))
        {
          QObject::connect(edit, SIGNAL(stateChanged(int)), SLOT(on_checkBox_stateChanged(int)));
        }
        else if (QComboBox* edit = qobject_cast<QComboBox*>(widget))
        {
          QObject::connect(edit, SIGNAL(editTextChanged(const QString&)), SLOT(on_lineEdit_textEdited(const QString&)));
          QObject::connect(edit, SIGNAL(currentIndexChanged(const QString&)), SLOT(on_lineEdit_textEdited(const QString&)));          
        }        
      }
    }

    void CcdCardPage::updateWidgets()
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

      bool controlEnabled = !readOnly && Qt::Checked == ui_.paymentControlCheck->checkState();
      ui_.paymentTermTypeCombo->setEnabled(controlEnabled);      
      ui_.paymentTermDate->setEnabled(controlEnabled);
      ui_.paymentTermPeriodSpin->setEnabled(controlEnabled);

      controlEnabled = !readOnly && Qt::Checked == ui_.zeroTaxControlCheck->checkState();
      ui_.zeroTaxPeriodCombo->setEnabled(controlEnabled);

      bool peristance = createMode != currentMode && entityId();      
      int generalTabIndex = ui_.tabWidget->indexOf(ui_.generalTab);      
      ui_.tabWidget->setTabEnabled(generalTabIndex, peristance);            
      switch (currentMode)
      {
      case viewMode:
        if (entityId())
        {
          updateWindowTitle(tr("CCD - %1").arg(entityData_.number));
        }
        showRequiredMarkers(false);
        break;

      case editMode:
        if (entityId())
        {
          if (contentsChanged_)
          {
            updateWindowTitle(tr("CCD - %1* - edit").arg(entityData_.number));
          }
          else
          {
            updateWindowTitle(tr("CCD - %1 - edit").arg(entityData_.number));
          }        
        }
        showRequiredMarkers(true);
        break;

      case createMode:
        updateWindowTitle(tr("CCD - New CCD creation*"));
        showRequiredMarkers(true);
        break;
      }
    }

    bool CcdCardPage::populateFromWidgets()
    {
      static const char* ccdNumberTitle = QT_TR_NOOP("CCD number");
      static const char* productCodeTitle = QT_TR_NOOP("Product code");
      static const char* contractTitle = QT_TR_NOOP("Contract");
      static const char* amountTitle = QT_TR_NOOP("Amount");            
      static const char* deppointTitle = QT_TR_NOOP("Departure point");
      static const char* ttermTitle = QT_TR_NOOP("Transaction terms");
      static const char* dutyTitle = QT_TR_NOOP("Duty");
      static const char* rateTitle = QT_TR_NOOP("Rate");
      static const char* seriesTitle = QT_TR_NOOP("Series");
      static const char* zeroTaxPeriodTitle = QT_TR_NOOP("Zero tax period");            
      
      static const char* messageTitle = QT_TR_NOOP("Invalid data input");
      static const char* notEmpty = QT_TR_NOOP("Field \"%1\" can\'t be empty.");
      static const char* tooLongText = QT_TR_NOOP("Field \"%1\" can\'t exceed %2 characters.");      
      static const char* invalidFieldValueText = QT_TR_NOOP("Invalid value in field \"%1\"."); 
      static const char* invalidInteger = QT_TR_NOOP("Field \"%1\" must be an integer value: [%2, %3].");
      
      int contentsTabIndex = ui_.tabWidget->indexOf(ui_.contentsTab);
      int paymentTabIndex = ui_.tabWidget->indexOf(ui_.paymentTab);
      int seriesTabIndex = ui_.tabWidget->indexOf(ui_.seriesTab);      
      bool validated;
      // Get ccd number
      QString ccdNumber = ui_.ccdNumberEdit->displayText();
      if (ccdNumber.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(ccdNumberTitle)));
      } 
      else if (ccdNumber.length() > model::Ccd::numberMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(ccdNumberTitle)).arg(model::Ccd::numberMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.ccdNumberEdit->setFocus(Qt::TabFocusReason);
        ui_.ccdNumberEdit->selectAll();
        return false;
      }
      // Get product code      
      QString productCode = ui_.productCodeEdit->text().trimmed();
      if (productCode.length() > model::Ccd::productCodeMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(productCodeTitle)).arg(model::Ccd::productCodeMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.productCodeEdit->setFocus(Qt::TabFocusReason);
        ui_.productCodeEdit->selectAll();
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
      // Get deppoint
      if (!selectedDeppointId_)
      {        
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(deppointTitle)));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.deppointBtn->setFocus(Qt::TabFocusReason);
        return false;
      }
      // Get tterm
      if (!selectedTtermId_)
      {       
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(ttermTitle)));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.ttermBtn->setFocus(Qt::TabFocusReason);
        return false;
      }
      // Get exportPermDate
      QDate exportPermdate = ui_.exportPermDate->date();
      // Get export date
      OptionalQDate exportDate;
      if (Qt::Checked == ui_.exportCheck->checkState())
      {
        exportDate.reset(ui_.exportDate->date());
      }                  
      // Get duty
      OptionalQInt64 duty;
      QString dutyText = ui_.dutyEdit->text().trimmed();
      if (!dutyText.isEmpty())
      {
        model::CurrencyAmount dutyAmount;
        if (!dutyAmount.setValue(dutyText))
        {
          QMessageBox::warning(this, tr(messageTitle), tr(invalidFieldValueText).arg(tr(dutyTitle)));
          ui_.tabWidget->setCurrentIndex(contentsTabIndex);
          ui_.dutyEdit->setFocus(Qt::TabFocusReason);
          ui_.dutyEdit->selectAll();
          return false;
        }
        duty.reset(dutyAmount.amount());
      }
      // Get rate
      OptionalQInt64 rate;
      QString rateText = ui_.rateEdit->text().trimmed();
      if (!rateText.isEmpty())
      {
        model::RateAmount rateAmount;
        if (!rateAmount.setValue(rateText))
        {
          QMessageBox::warning(this, tr(messageTitle), tr(invalidFieldValueText).arg(tr(rateTitle)));
          ui_.tabWidget->setCurrentIndex(contentsTabIndex);
          ui_.rateEdit->setFocus(Qt::TabFocusReason);
          ui_.rateEdit->selectAll();
          return false;
        }
        rate.reset(rateAmount.amount());
      }
      bool peristance = createMode != mode() && entityId();
      // Get zero tax period
      int zeroTaxPeriod = 0;      
      if (Qt::Checked == ui_.zeroTaxControlCheck->checkState())
      {      
        zeroTaxPeriod = ui_.zeroTaxPeriodCombo->currentText().trimmed().toInt(&validated);
        if (validated)
        {
          validated = zeroTaxPeriod >= model::Ccd::zeroTaxPeriodMin && zeroTaxPeriod <= model::Ccd::zeroTaxPeriodMax;        
        }
        if (!validated)
        {
          QMessageBox::warning(this, tr(messageTitle), 
            tr(invalidInteger).arg(tr(zeroTaxPeriodTitle)).arg(model::Ccd::zeroTaxPeriodMin).arg(model::Ccd::zeroTaxPeriodMax));
          ui_.tabWidget->setCurrentIndex(paymentTabIndex);
          ui_.zeroTaxPeriodCombo->setFocus(Qt::TabFocusReason);
          ui_.zeroTaxPeriodCombo->lineEdit()->selectAll();
          return false;
        }
      }
      else if (peristance)
      {
        zeroTaxPeriod = entityData_.zeroTaxPeriod;
      }
      // Get payment term data
      int paymentTermType = 0;
      OptionalQDate paymentTermDate(QDate::currentDate());
      OptionalInt   paymentTermPeriod;
      if (Qt::Checked == ui_.paymentControlCheck->checkState())
      {              
        paymentTermType = ui_.paymentTermTypeCombo->currentIndex();        
        switch (paymentTermType)
        {
        case 1:
          paymentTermPeriod.reset(ui_.paymentTermPeriodSpin->value());
          break;
        case 0:
        default:
          paymentTermDate.reset(ui_.paymentTermDate->date());
          break;
        }
      }
      else if (peristance)
      {
        paymentTermType = entityData_.paymentTermType;
        paymentTermDate = entityData_.paymentTermDate;
        paymentTermPeriod = entityData_.paymentTermPeriod;
      }
      // Get series
      QString series = ui_.seriesTextEdit->toPlainText();      
      if (series.length() > model::Ccd::seriesMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(seriesTitle)).arg(model::Ccd::seriesMaxLen));                
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(seriesTabIndex);
        ui_.seriesTextEdit->setFocus(Qt::TabFocusReason);
        ui_.seriesTextEdit->selectAll();
        return false;
      }
                 
      entityData_.number      = ccdNumber;      
      entityData_.contractId  = selectedContractId_.get();      
      entityData_.deppointId  = selectedDeppointId_.get();      
      entityData_.exportPermDate = exportPermdate;      
      entityData_.exportDate  = exportDate;
      entityData_.productCode = productCode.isNull() ? OptionalQString() : productCode;
      entityData_.duty        = duty;
      entityData_.rate        = rate;
      entityData_.series      = series.isNull() ? OptionalQString() : series;
      entityData_.ttermId     = selectedTtermId_.get();
      entityData_.amount      = currencyAmount.amount();
      entityData_.zeroTaxPeriod = zeroTaxPeriod;
      entityData_.paymentTermType = paymentTermType;
      entityData_.paymentTermDate = paymentTermDate;
      entityData_.paymentTermPeriod = paymentTermPeriod;
      entityData_.paymentControlFlag = Qt::Checked == ui_.paymentControlCheck->checkState();
      entityData_.zeroTaxControlFlag = Qt::Checked == ui_.zeroTaxControlCheck->checkState();
      return true;
    }

    bool CcdCardPage::populateFromDatabase()
    {      
      static const char* querySql = "select " \
        "t.\"NUMBER\", " \
        "t.\"EXPORT_PERM_DATE\", " \
        "t.\"EXPORT_DATE\", " \
        "t.\"PRODUCT_CODE\", t.\"DUTY\", t.\"SERIES\", t.\"AMOUNT\", " \
        "t.\"CREATE_USER_ID\", cu.\"LOGIN\", " \
        "t.\"UPDATE_USER_ID\", uu.\"LOGIN\", " \
        "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", " \
        "t.\"CONTRACT_ID\", con.\"NUMBER\", con.\"PASSPORT_NUMBER\", " \
        "t.\"DEPPOINT_ID\", dp.\"SHORT_NAME\", " \
        "t.\"TTERM_ID\", tt.\"NAME\", " \
        "con.\"BANK_ID\", b.\"SHORT_NAME\", " \
        "con.\"COUNTERPART_ID\", con.\"CURRENCY_ID\", " \
        "cnt.\"SHORT_NAME\", cur.\"FULL_NAME\", t.\"RATE\", " \
        "t.\"ZERO_TAX_PERIOD\", " \
        "t.\"PAYMENT_TERM_TYPE\", t.\"PAYMENT_TERM_DATE\", t.\"PAYMENT_TERM_PERIOD\", " \
        "t.\"PAYMENT_LIMIT_DATE\", t.\"ZERO_TAX_LIMIT_DATE\", " \
        "t.\"PAYMENT_CONTROL_FLAG\", t.\"ZERO_TAX_CONTROL_FLAG\" " \
        "from \"CCD_SEL\" t " \
        "join \"CONTRACT\" con on t.\"CONTRACT_ID\" = con.\"CONTRACT_ID\" " \
        "join \"DEPPOINT\" dp on t.\"DEPPOINT_ID\" = dp.\"DEPPOINT_ID\" " \
        "join \"TTERM\" tt on t.\"TTERM_ID\" = tt.\"TTERM_ID\" " \
        "join \"BANK\" b on con.\"BANK_ID\" = b.\"BANK_ID\" " \
        "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
        "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
        "join \"COUNTERPART\" cnt on con.\"COUNTERPART_ID\" = cnt.\"COUNTERPART_ID\" " \
        "join \"CURRENCY\" cur on con.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
        "where t.\"CCD_ID\" = ?";

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
              entityData_.ccdId   = entityId().get();
              entityData_.number  = query.value(0).toString();              
              entityData_.exportPermDate = databaseModel()->convertFromServer(query.value(1).toDate());              
              entityData_.exportDate     = query.isNull(2) ? OptionalQDate() : databaseModel()->convertFromServer(query.value(2).toDate());
              entityData_.productCode    = query.isNull(3) ? OptionalQString() : query.value(3).toString();
              entityData_.duty   = query.isNull(4) ? OptionalQInt64() : query.value(4).toLongLong();
              entityData_.series = query.isNull(5) ? OptionalQString() : query.value(5).toString();
              entityData_.amount = query.value(6).toLongLong();
              entityData_.createUserId = query.value(7).toLongLong();
              createUserName_          = query.value(8).toString(); 
              entityData_.updateUserId = query.value(9).toLongLong();
              updateUserName_          = query.value(10).toString(); 
              entityData_.createTime   = databaseModel()->convertFromServer(query.value(11).toDateTime()); 
              entityData_.updateTime   = databaseModel()->convertFromServer(query.value(12).toDateTime()); 
              entityData_.contractId   = query.value(13).toLongLong();
              contractNumber_          = query.value(14).toString();
              contractPassportNumber_  = query.isNull(15) ? OptionalQString() : query.value(15).toString();
              entityData_.deppointId   = query.value(16).toLongLong();
              deppointShortName_       = query.value(17).toString();
              entityData_.ttermId      = query.value(18).toLongLong();
              ttermShortName_          = query.value(19).toString();
              bankId_                  = query.value(20).toLongLong();
              bankShortName_           = query.value(21).toString();
              counterpartId_           = query.value(22).toLongLong();
              currencyId_              = query.value(23).toLongLong();
              counterpartShortName_    = query.value(24).toString();
              currencyFullName_        = query.value(25).toString();
              entityData_.rate = query.isNull(26) ? OptionalQInt64() : query.value(26).toLongLong();
              entityData_.zeroTaxPeriod = query.value(27).toInt();
              entityData_.paymentTermType = query.value(28).toInt();
              entityData_.paymentTermDate = 0 == entityData_.paymentTermType ? databaseModel()->convertFromServer(query.value(29).toDate()) : OptionalQDate();
              entityData_.paymentTermPeriod = 0 == entityData_.paymentTermType ? OptionalInt() : query.value(30).toInt();
              paymentLimitDate_ = query.isNull(31) ? OptionalQDate() : databaseModel()->convertFromServer(query.value(31).toDate());
              zeroTaxLimitDate_ = databaseModel()->convertFromServer(query.value(32).toDate());
              entityData_.paymentControlFlag = 0 != query.value(33).toInt();
              entityData_.zeroTaxControlFlag = 0 != query.value(34).toInt();
              return true;
            }
          }
        }
      }
      return false;
    }

    void CcdCardPage::populateWidgets()
    {
      if (entityId())
      {        
        // Contents page
        ui_.ccdNumberEdit->setText(entityData_.number);
        ui_.contractEdit->setText(contractNumber_);    
        ui_.passportNumberEdit->setText(contractPassportNumber_ ? contractPassportNumber_.get() : QString());
        selectedContractId_ = entityData_.contractId;        
        ui_.amountEdit->setText(model::CurrencyAmount(entityData_.amount).toString());
        ui_.exportPermDate->setDate(entityData_.exportPermDate);        
        ui_.exportCheck->setCheckState(entityData_.exportDate ? Qt::Checked : Qt::Unchecked);
        if (entityData_.exportDate)
        {
          ui_.exportDate->setDate(entityData_.exportDate.get());
          ui_.exportStackedWidget->setCurrentIndex(1);
        }
        else
        {
          ui_.exportStackedWidget->setCurrentIndex(0);
        }
        ui_.productCodeEdit->setText(entityData_.productCode ? entityData_.productCode.get() : QString());                
        ui_.counterpartEdit->setText(counterpartShortName_);        
        selectedCounterpartId_ = counterpartId_;
        ui_.currencyEdit->setText(currencyFullName_);        
        selectedCurrencyId_ = currencyId_;
        ui_.deppointEdit->setText(deppointShortName_);        
        selectedDeppointId_ = entityData_.deppointId;        
        ui_.bankEdit->setText(bankShortName_);        
        selectedBankId_ = bankId_;
        ui_.ttermEdit->setText(ttermShortName_);                
        selectedTtermId_ = entityData_.ttermId;
        ui_.dutyEdit->setText(entityData_.duty ? model::CurrencyAmount(entityData_.duty.get()).toString() : QString());        
        ui_.rateEdit->setText(entityData_.rate ? model::RateAmount(entityData_.rate.get()).toString() : QString());        
        // Payment control
        ui_.zeroTaxPeriodCombo->lineEdit()->setText(QString("%1").arg(entityData_.zeroTaxPeriod));
        switch (entityData_.paymentTermType)
        {
        case 1:
          ui_.paymentTermTypeCombo->setCurrentIndex(1);
          ui_.paymentTermStackedWidget->setCurrentIndex(1);
          ui_.paymentTermPeriodSpin->setValue(entityData_.paymentTermPeriod.get());
          break;

        case 0:
        default:
          ui_.paymentTermTypeCombo->setCurrentIndex(0);
          ui_.paymentTermStackedWidget->setCurrentIndex(0);
          ui_.paymentTermDate->setDate(entityData_.paymentTermDate.get());
          break;
        }    
        ui_.paymentControlCheck->setCheckState(entityData_.paymentControlFlag ? Qt::Checked : Qt::Unchecked);
        ui_.zeroTaxControlCheck->setCheckState(entityData_.zeroTaxControlFlag ? Qt::Checked : Qt::Unchecked);
        // Series page
        ui_.seriesTextEdit->setText(entityData_.series ? entityData_.series.get() : QString());
        // General data page
        ui_.createTimeEdit->setText(entityData_.createTime.toString(Qt::DefaultLocaleShortDate));
        ui_.updateTimeEdit->setText(entityData_.updateTime.toString(Qt::DefaultLocaleShortDate));
        ui_.createUserEdit->setText(createUserName_);
        ui_.updateUserEdit->setText(updateUserName_);         
        populateExportData();
        contentsChanged_ = false;
      }
    } 

    void CcdCardPage::populateExportData()
    {
      static const QString ccdNumberCaption = tr("CCD number");
      static const QString productCodeCaption = tr("Product code");
      static const QString amountCaption = tr("Amount");
      static const QString contractNumberCaption = tr("Contract");      
      static const QString passportNumberCaption = tr("Transaction passport");
      static const QString counterpartShortNameCaption = tr("Counterpart");
      static const QString bankShortNameCaption = tr("Bank");
      static const QString currencyFullNameCaption = tr("Currency");
      static const QString deppointShortNameCaption = tr("Departure point");
      static const QString exportPermDateCaption = tr("Export permissoion date");
      static const QString exportFlagCaption = tr("Exported");
      static const QString exportDateCaption = tr("Export date");
      static const QString ttermShortNameCaption = tr("Transaction terms");
      static const QString dutyCaption = tr("Duty");
      static const QString rateCaption = tr("Rate");      
      static const QString paymentTermTypeCaption = tr("Payment term type");      
      static const QString paymentTermPeriodCaption = tr("Payment term period");            
      static const QString zeroTaxPeriodCaption = tr("Zero tax period");
      static const QString paymentLimitDateCaption = tr("Payment limit date");
      static const QString zeroTaxLimitDateCaption = tr("Zero-tax limit date");
      static const QString seriesCaption = tr("Series");  
      static const QString createUserCaption = tr("User, creator");
      static const QString createTimeCaption = tr("Time, created");
      static const QString updateUserCaption = tr("User, last update");
      static const QString updateTimeCaption = tr("Time, last update");                    
        
      ExportDataList exportDataList;
      exportDataList << std::make_pair(ccdNumberCaption, QVariant(entityData_.number));
      exportDataList << std::make_pair(productCodeCaption, QVariant(entityData_.productCode ? entityData_.productCode.get() : QString()));
      exportDataList << std::make_pair(amountCaption, QVariant(entityData_.amount));
      exportDataList << std::make_pair(contractNumberCaption, QVariant(contractNumber_));
      exportDataList << std::make_pair(passportNumberCaption, QVariant(contractPassportNumber_ ? contractPassportNumber_.get() : QString()));
      exportDataList << std::make_pair(counterpartShortNameCaption, QVariant(counterpartShortName_));
      exportDataList << std::make_pair(bankShortNameCaption, QVariant(bankShortName_));
      exportDataList << std::make_pair(currencyFullNameCaption, QVariant(currencyFullName_));
      exportDataList << std::make_pair(deppointShortNameCaption, QVariant(deppointShortName_));
      exportDataList << std::make_pair(exportPermDateCaption, QVariant(entityData_.exportPermDate));
      exportDataList << std::make_pair(exportFlagCaption, QVariant(entityData_.exportDate.is_initialized()));
      exportDataList << std::make_pair(exportDateCaption, entityData_.exportDate ? QVariant(entityData_.exportDate.get()) : QVariant(QVariant::Date));
      exportDataList << std::make_pair(ttermShortNameCaption, QVariant(ttermShortName_));
      exportDataList << std::make_pair(dutyCaption, entityData_.duty ? QVariant::fromValue(model::CurrencyAmount(entityData_.duty.get())) : QVariant(QString()));
      exportDataList << std::make_pair(rateCaption, entityData_.rate ? QVariant::fromValue(model::RateAmount(entityData_.rate.get())) : QVariant(QString()));            
      switch (entityData_.paymentTermType)
      {
      case 1:
        exportDataList << std::make_pair(paymentTermTypeCaption, QVariant(ui_.paymentTermTypeCombo->itemText(1)));
        exportDataList << std::make_pair(paymentTermPeriodCaption, QVariant(entityData_.paymentTermPeriod.get()));              
        break;      
      default:
        exportDataList << std::make_pair(paymentTermTypeCaption, QVariant(ui_.paymentTermTypeCombo->itemText(0)));
        break;
      }
      exportDataList << std::make_pair(paymentLimitDateCaption, paymentLimitDate_ ? QVariant(paymentLimitDate_.get()) : QVariant(QVariant::Date));
      exportDataList << std::make_pair(zeroTaxPeriodCaption, QVariant(entityData_.zeroTaxPeriod));            
      exportDataList << std::make_pair(zeroTaxLimitDateCaption, QVariant(zeroTaxLimitDate_));      
      exportDataList << std::make_pair(seriesCaption, entityData_.series ? QVariant(entityData_.series.get()) : QVariant(QVariant::String));      
      exportDataList << std::make_pair(createUserCaption, QVariant(createUserName_));
      exportDataList << std::make_pair(createTimeCaption, QVariant(entityData_.createTime));
      exportDataList << std::make_pair(updateUserCaption, QVariant(updateUserName_));
      exportDataList << std::make_pair(updateTimeCaption, QVariant(entityData_.updateTime));      

      setExportDataList(exportDataList);
    }

    bool CcdCardPage::updateDatabase()
    {
      static const char* querySql = "update \"CCD\" set " \
        "\"NUMBER\" = ?, \"CONTRACT_ID\" = ?, " \
        "\"DEPPOINT_ID\" = ?, " \
        "\"EXPORT_PERM_DATE\" = ?, " \
        "\"EXPORT_DATE\" = ?, " \
        "\"PRODUCT_CODE\" = ?, \"DUTY\" = ?, " \
        "\"SERIES\" = ?, \"TTERM_ID\" = ?, " \
        "\"AMOUNT\" = ?, " \
        "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\', \"RATE\" = ?, " \
        "\"ZERO_TAX_PERIOD\" = ?, " \
        "\"PAYMENT_TERM_TYPE\" = ?, \"PAYMENT_TERM_DATE\" = ?, \"PAYMENT_TERM_PERIOD\" = ?, " \
        "\"PAYMENT_CONTROL_FLAG\" = ?, \"ZERO_TAX_CONTROL_FLAG\" = ? " \
        "where \"CCD_ID\" = ?";

      if (entityId())
      {        
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {          
          query.bindValue(0, entityData_.number);          
          query.bindValue(1, entityData_.contractId);          
          query.bindValue(2, entityData_.deppointId);          
          query.bindValue(3, entityData_.exportPermDate);          
          query.bindValue(4, entityData_.exportDate ? entityData_.exportDate.get() : QVariant(QVariant::Date));
          query.bindValue(5, entityData_.productCode ? entityData_.productCode.get() : QVariant(QVariant::String));
          query.bindValue(6, entityData_.duty ? entityData_.duty.get() : QVariant(QVariant::LongLong));          
          query.bindValue(7, entityData_.series ? entityData_.series.get() : QVariant(QVariant::String));
          query.bindValue(8, entityData_.ttermId);
          query.bindValue(9, entityData_.amount);
          query.bindValue(10, databaseModel()->userId());
          query.bindValue(11, entityData_.rate ? entityData_.rate.get() : QVariant(QVariant::LongLong));
          query.bindValue(12, entityData_.zeroTaxPeriod);
          query.bindValue(13, entityData_.paymentTermType);          
          switch (entityData_.paymentTermType)
          {
          case 1:
            query.bindValue(14, QVariant(QVariant::Date));
            query.bindValue(15, entityData_.paymentTermPeriod.get());
            break;
          case 0:
          default:
            query.bindValue(14, entityData_.paymentTermDate.get());
            query.bindValue(15, QVariant(QVariant::Int));
            break;
          }
          query.bindValue(16, QVariant(entityData_.paymentControlFlag ? 1 : 0));
          query.bindValue(17, QVariant(entityData_.zeroTaxControlFlag ? 1 : 0));
          query.bindValue(18, entityData_.ccdId);
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

    bool CcdCardPage::insertIntoDatabase()
    {
      static const char* querySql = "insert into \"CCD\" (" \
        "\"CCD_ID\", \"NUMBER\", \"CONTRACT_ID\", " \
        "\"DEPPOINT_ID\", " \
        "\"EXPORT_PERM_DATE\", " \
        "\"EXPORT_DATE\", " \
        "\"PRODUCT_CODE\", \"DUTY\", " \
        "\"SERIES\", \"TTERM_ID\", " \
        "\"AMOUNT\", " \
        "\"CREATE_USER_ID\", \"UPDATE_USER_ID\", \"CREATE_TIME\", \"UPDATE_TIME\", \"RATE\", " \
        "\"ZERO_TAX_PERIOD\", " \
        "\"PAYMENT_TERM_TYPE\", \"PAYMENT_TERM_DATE\", \"PAYMENT_TERM_PERIOD\", " \
        "\"PAYMENT_CONTROL_FLAG\", \"ZERO_TAX_CONTROL_FLAG\") " \
        "values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, \'NOW\', \'NOW\', ?, ?, ?, ?, ?, ?, ?)";      

      if (OptionalQInt64 newEntityId = databaseModel()->generateId(ccdEntity))
      {
        entityData_.ccdId = newEntityId.get();    
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.ccdId);
          query.bindValue(1, entityData_.number);          
          query.bindValue(2, entityData_.contractId);          
          query.bindValue(3, entityData_.deppointId);          
          query.bindValue(4, entityData_.exportPermDate);          
          query.bindValue(5, entityData_.exportDate ? entityData_.exportDate.get() : QVariant(QVariant::Date));
          query.bindValue(6, entityData_.productCode ? entityData_.productCode.get() : QVariant(QVariant::String));
          query.bindValue(7, entityData_.duty ? entityData_.duty.get() : QVariant(QVariant::LongLong));
          query.bindValue(8, entityData_.series ? entityData_.series.get() : QVariant(QVariant::String));
          query.bindValue(9, entityData_.ttermId);
          query.bindValue(10, entityData_.amount);
          query.bindValue(11, databaseModel()->userId());          
          query.bindValue(12, databaseModel()->userId()); 
          query.bindValue(13, entityData_.rate ? entityData_.rate.get() : QVariant(QVariant::LongLong));
          query.bindValue(14, entityData_.zeroTaxPeriod);
          query.bindValue(15, entityData_.paymentTermType);
          switch (entityData_.paymentTermType)
          {
          case 1:
            query.bindValue(16, QVariant(QVariant::Date));
            query.bindValue(17, entityData_.paymentTermPeriod.get());
            break;
          case 0:
          default:
            query.bindValue(16, entityData_.paymentTermDate.get());
            query.bindValue(17, QVariant(QVariant::Int));
            break;
          }
          query.bindValue(18, QVariant(entityData_.paymentControlFlag ? 1 : 0));
          query.bindValue(19, QVariant(entityData_.zeroTaxControlFlag ? 1 : 0));
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

    bool CcdCardPage::removeFromDatabase()
    {
      static const char* querySql = "delete from \"CCD\" where \"CCD_ID\" = ?";

      if (QMessageBox::Yes == QMessageBox::question(this, tr("Ccds"), 
        tr("Delete ccd?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
      {
        if (entityId())
        {
          QSqlQuery query(databaseModel()->database());              
          if (query.prepare(querySql))
          {
            query.bindValue(0, entityData_.ccdId);
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

    void CcdCardPage::on_lineEdit_textEdited(const QString&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void CcdCardPage::on_textEdit_textChanged()
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void CcdCardPage::on_dateEdit_dateChanged(const QDate&)
    {
      contentsChanged_ = true;
      updateWidgets();
    } 
    
    void CcdCardPage::on_checkBox_stateChanged(int)
    {
      contentsChanged_ = true;
      updateWidgets();
    }
    
    void CcdCardPage::on_counterpartBtn_clicked(bool /*checked*/)
    {
      if (selectedCounterpartId_)
      {
        showCounterpartCardTaskWindow(selectedCounterpartId_.get());
      }
    }

    void CcdCardPage::on_currencyBtn_clicked(bool /*checked*/)
    {      
      if (selectedCurrencyId_)
      {          
        showCurrencyCardTaskWindow(selectedCurrencyId_.get());
      }
    }

    void CcdCardPage::on_contractBtn_clicked(bool /*checked*/)
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

    void CcdCardPage::on_deppointBtn_clicked(bool /*checked*/)
    {
      if (viewMode == mode())
      {
        if (entityId())
        {          
          showDeppointCardTaskWindow(entityData_.deppointId);
        }        
      }
      else
      {
        //todo: add default selection
        std::auto_ptr<ListPage> listPage(new DeppointListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), 
          UserDefinedFilter(), UserDefinedOrder(), 0));
        ListSelectDialog listSelectDialog(this, listPage);
        listSelectDialog.setWindowModality(Qt::WindowModal);
        if (QDialog::Accepted == listSelectDialog.exec())
        {          
          selectedDeppointId_ = listSelectDialog.listPage()->selectedId();          
          if (OptionalQString deppointShortName = listSelectDialog.listPage()->selectedRelationalText())
          {
            ui_.deppointEdit->setText(deppointShortName.get());
          }
          contentsChanged_ = true;
          updateWidgets();
        }
      } 
    }

    void CcdCardPage::on_bankBtn_clicked(bool /*checked*/)
    {      
      if (selectedBankId_)
      {          
        showBankCardTaskWindow(selectedBankId_.get());
      }
    }

    void CcdCardPage::on_ttermBtn_clicked(bool /*checked*/)
    {
      if (viewMode == mode())
      {
        if (entityId())
        {          
          showTtermCardTaskWindow(entityData_.ttermId);
        }        
      }
      else
      {
        //todo: add default selection
        std::auto_ptr<ListPage> listPage(new TtermListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), 
          UserDefinedFilter(), UserDefinedOrder(), 0));
        ListSelectDialog listSelectDialog(this, listPage);
        listSelectDialog.setWindowModality(Qt::WindowModal);
        if (QDialog::Accepted == listSelectDialog.exec())
        {          
          selectedTtermId_ = listSelectDialog.listPage()->selectedId();          
          if (OptionalQString ttermShortName = listSelectDialog.listPage()->selectedRelationalText())
          {
            ui_.ttermEdit->setText(ttermShortName.get());
          }
          contentsChanged_ = true;
          updateWidgets();
        }
      } 
    }

    void CcdCardPage::on_createUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.createUserId);
      }        
    }

    void CcdCardPage::on_updateUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.updateUserId);
      }        
    }

    void CcdCardPage::on_exportCheck_stateChanged(int state)
    {
      if (Qt::Checked == state)
      {
        ui_.exportStackedWidget->setCurrentIndex(1);
      }
      else
      {
        ui_.exportStackedWidget->setCurrentIndex(0);
      }      
    }    

    void CcdCardPage::on_paymentTermTypeCombo_currentIndexChanged(int index)
    {
      switch (index)
      {
      case 1:
        ui_.paymentTermStackedWidget->setCurrentIndex(1);
        break;

      case 0:
      default:
        ui_.paymentTermStackedWidget->setCurrentIndex(0);
        break;
      }
    }

    void CcdCardPage::on_database_userUpdated(const ma::chrono::model::User& user)
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

    void CcdCardPage::on_database_counterpartUpdated(const ma::chrono::model::Counterpart& counterpart)
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

    void CcdCardPage::on_database_currencyUpdated(const ma::chrono::model::Currency& currency)
    {
      if (entityId())
      {        
        if (currencyId_ == currency.currencyId)
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

    void CcdCardPage::on_database_bankUpdated(const ma::chrono::model::Bank& bank)
    {
      if (entityId())
      {
        if (bankId_ == bank.bankId)
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

    void CcdCardPage::on_database_contractUpdated(const ma::chrono::model::Contract& contract)
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
            ui_.passportNumberEdit->setText(contractPassportNumber_ ? contractPassportNumber_.get() : QString());              
            selectedBankId_ = bankId_;
            ui_.bankEdit->setText(bankShortName_);
            selectedCounterpartId_ = counterpartId_;
            ui_.counterpartEdit->setText(counterpartShortName_);
            selectedCurrencyId_ = currencyId_;
            ui_.currencyEdit->setText(currencyFullName_);            
          }
          else
          {
            updateContractRelatedNonPersistData(contract.contractId);
          }
          updateWidgets();
        }
      }      
    }

    void CcdCardPage::on_database_contractRemoved(const ma::chrono::model::Contract& contract)
    {
      if (selectedContractId_)
      {
        if (selectedContractId_.get() == contract.contractId)
        {
          ui_.contractEdit->setText(QString());
          ui_.passportNumberEdit->setText(QString());
          selectedContractId_.reset();
          ui_.bankEdit->setText(QString());
          selectedBankId_.reset();
          ui_.counterpartEdit->setText(QString());
          selectedCounterpartId_.reset();
          ui_.currencyEdit->setText(QString());
          selectedCurrencyId_.reset();
          updateWidgets();
        }
      }
    }    

    void CcdCardPage::on_database_deppointUpdated(const ma::chrono::model::Deppoint& deppoint)
    {
      if (entityId())
      {
        if (entityData_.deppointId == deppoint.deppointId)
        {
          deppointShortName_ = deppoint.shortName;          
        }
      }
      if (selectedDeppointId_)
      {
        if (selectedDeppointId_.get() == deppoint.deppointId)
        {
          ui_.deppointEdit->setText(deppoint.shortName);
          updateWidgets();
        }
      }
    }

    void CcdCardPage::on_database_deppointRemoved(const ma::chrono::model::Deppoint& deppoint)
    {
      if (selectedDeppointId_)
      {
        if (selectedDeppointId_.get() == deppoint.deppointId)
        {
          ui_.deppointEdit->setText(QString());
          selectedDeppointId_.reset();
          updateWidgets();
        }
      }
    }      

    void CcdCardPage::on_database_ttermUpdated(const ma::chrono::model::Tterm& tterm)
    {
      if (entityId())
      {
        if (entityData_.ttermId == tterm.ttermId)
        {
          ttermShortName_ = tterm.name;
        }
      }
      if (selectedTtermId_)
      {
        if (selectedTtermId_.get() == tterm.ttermId)
        {
          ui_.ttermEdit->setText(tterm.name);          
          updateWidgets();
        }
      }
    }

    void CcdCardPage::on_database_ttermRemoved(const ma::chrono::model::Tterm& tterm)
    {
      if (selectedTtermId_)
      {
        if (selectedTtermId_.get() == tterm.ttermId)
        {
          ui_.ttermEdit->setText(QString());
          selectedTtermId_.reset();
          updateWidgets();
        }
      }
    }
    
    void CcdCardPage::updateContractRelatedPersistData(qint64 contractId)
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
              contractNumber_         = query.value(0).toString();
              contractPassportNumber_ = query.value(1).isNull() ? OptionalQString() : query.value(1).toString();
              bankId_                 = query.value(2).toLongLong();
              bankShortName_ = query.value(3).toString();
              counterpartId_ = query.value(4).toLongLong();
              counterpartShortName_ = query.value(6).toString();
              currencyId_           = query.value(5).toLongLong();
              currencyFullName_ = query.value(7).toString();
            }
          }
        }
      }
    }

    void CcdCardPage::updateContractRelatedNonPersistData(qint64 contractId)
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
              ui_.passportNumberEdit->setText(query.value(1).toString());              
              selectedBankId_ = query.value(2).toLongLong();
              ui_.bankEdit->setText(query.value(3).toString());
              selectedCounterpartId_ = query.value(4).toLongLong();
              ui_.counterpartEdit->setText(query.value(6).toString());
              selectedCurrencyId_ = query.value(5).toLongLong();
              ui_.currencyEdit->setText(query.value(7).toString());              
            }
          }
        }
      }
    }

    void CcdCardPage::showRequiredMarkers(bool visible)
    {
      WidgetUtility::show(requiredMarkers_, visible);
    }

  } // namespace chrono
} // namespace ma
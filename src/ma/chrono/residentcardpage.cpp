/*
TRANSLATOR ma::chrono::ResidentCardPage
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <limits>
#include <QAction>
#include <QMessageBox>
#include <QCalendarWidget>
#include <QSqlQuery>
#include <ma/chrono/constants.h>
#include <ma/chrono/dbconstants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/widgetutility.h>
#include <ma/chrono/sqlutility.h>
#include <ma/chrono/listselectdialog.h>
#include <ma/chrono/countrylistpage.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/country.h>
#include <ma/chrono/localeutility.h>
#include <ma/chrono/residentcardpage.h>

namespace ma
{
  namespace chrono
  {    
    ResidentCardPage::ResidentCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent)
      : CardPage(resourceManager, taskWindowManager, databaseModel, helpAction, residentEntity, mode, entityId, parent)
      , dataAwareWidgets_()
      , contentsChanged_(false)
    {
      ui_.setupUi(this);
      ui_.tabWidget->setCurrentIndex(0);
      LocaleUtility::setupLocaleAwareWidget(ui_.sreDateEdit);
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      createAction()->setText(tr("&New resident"));
      dataAwareWidgets_ << ui_.fullNameEdit << ui_.shortNameEdit << ui_.regionEdit
                        << ui_.registryNumberEdit << ui_.intbEdit << ui_.crsaEdit2 
                        << ui_.cityEdit << ui_.streetEdit << ui_.zipEdit
                        << ui_.houseNumberEdit << ui_.advHouseNumberEdit << ui_.caseNumberEdit
                        << ui_.officeNumberEdit << ui_.advOfficeNumberEdit << ui_.sreDateEdit;
      requiredMarkers_  << ui_.fullNameAsterikLabel << ui_.shortNameAsterikLabel << ui_.countryAsterikLabel;
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
      QObject::connect(databaseModel.get(), SIGNAL(countryUpdated(const ma::chrono::model::Country&)), SLOT(on_database_countryUpdated(const ma::chrono::model::Country&)));
      QObject::connect(databaseModel.get(), SIGNAL(countryRemoved(const ma::chrono::model::Country&)), SLOT(on_database_countryRemoved(const ma::chrono::model::Country&)));
      updateWidgets();
    }    

    ResidentCardPage::~ResidentCardPage()
    {
    }

    void ResidentCardPage::refresh()
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

    void ResidentCardPage::save()
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

    void ResidentCardPage::cancel()
    {
      if (editMode == mode())
      {      
        populateWidgets();
      }      
      CardPage::cancel();
      updateWidgets();
    }

    void ResidentCardPage::edit()
    {      
      CardPage::edit();
      updateWidgets();
    }

    void ResidentCardPage::remove()
    {
      if (createMode != mode())
      {
        if (removeFromDatabase())
        {
          CardPage::remove();
        }
      }            
    }

    std::auto_ptr<CardPage> ResidentCardPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new ResidentCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }           

    void ResidentCardPage::connectDataAwareWidgets()
    {
      
      typedef QWidgetList::const_iterator const_iterator;
      
      for (const_iterator it = dataAwareWidgets_.begin(), end = dataAwareWidgets_.end(); it != end; ++it)
      {
        QWidget* widget = *it;        
        if (QLineEdit* edit = qobject_cast<QLineEdit*>(widget))
        {
          QObject::connect(edit, SIGNAL(textEdited(const QString&)), SLOT(on_lineEdit_textEdited(const QString&)));
        }
        else if (QDateEdit* edit = qobject_cast<QDateEdit*>(widget))
        {
          QObject::connect(edit, SIGNAL(dateChanged(const QDate&)), SLOT(on_dateEdit_dateChanged(const QDate&)));
        }
      }
    }

    void ResidentCardPage::updateWidgets()
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
          updateWindowTitle(tr("Residents - %1").arg(entityData_.shortName));
        }
        showRequiredMarkers(false);
        break;

      case editMode:
        if (entityId())
        {
          if (contentsChanged_)
          {
            updateWindowTitle(tr("Residents - %1* - edit").arg(entityData_.shortName));
          }
          else
          {
            updateWindowTitle(tr("Residents - %1 - edit").arg(entityData_.shortName));
          }        
        }
        showRequiredMarkers(true);
        break;

      case createMode:
        updateWindowTitle(tr("Residents - New resident creation*"));
        showRequiredMarkers(true);
        break;
      }      
    }

    bool ResidentCardPage::populateFromWidgets()
    { 
      static const char* fullNameTitle = QT_TR_NOOP("Full name");
      static const char* shortNameTitle = QT_TR_NOOP("Short name");
      static const char* regionNameTitle = QT_TR_NOOP("Region name");
      static const char* cityNameTitle = QT_TR_NOOP("City name");
      static const char* streetNameTitle = QT_TR_NOOP("Street name");
      static const char* houseNumberTitle = QT_TR_NOOP("House number");
      static const char* advHouseNumberTitle = QT_TR_NOOP("Advanced house number");
      static const char* caseNumberTitle = QT_TR_NOOP("Case number");
      static const char* officeNumberTitle = QT_TR_NOOP("Office number");
      static const char* advOfficeNumberTitle = QT_TR_NOOP("Advanced office number");
      static const char* zipCodeTitle = QT_TR_NOOP("ZIP code");
      static const char* registryNumberTitle = QT_TR_NOOP("Registry number");
      static const char* intbTitle = QT_TR_NOOP("INTB");
      static const char* crsaTitle = QT_TR_NOOP("CRSA");
      static const char* countryTitle = QT_TR_NOOP("Country");
      
      static const char* messageTitle = QT_TR_NOOP("Invalid data input");
      static const char* notEmpty = QT_TR_NOOP("Field \"%1\" can\'t be empty.");
      static const char* tooLongText = QT_TR_NOOP("Field \"%1\" can\'t exceed %2 characters.");
      static const char* invalidInteger = QT_TR_NOOP("Field \"%1\" must be an integer value: [%2, %3].");

      int contentsTabIndex = ui_.tabWidget->indexOf(ui_.contentsTab);
      bool validated;
      // Get full name
      QString fullName = ui_.fullNameEdit->text().trimmed();
      if (fullName.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(fullNameTitle)));
      } 
      else if (fullName.length() > model::Resident::fullNameMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(fullNameTitle)).arg(model::Resident::fullNameMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.fullNameEdit->setFocus(Qt::TabFocusReason);
        ui_.fullNameEdit->selectAll();
        return false;
      }
      // Get short name
      QString shortName = ui_.shortNameEdit->text().trimmed();
      if (shortName.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(shortNameTitle)));
      } 
      else if (shortName.length() > model::Resident::shortNameMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(shortNameTitle)).arg(model::Resident::shortNameMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.shortNameEdit->setFocus(Qt::TabFocusReason);
        ui_.shortNameEdit->selectAll();
        return false;
      }
      // Get registry number
      QString registryNumber = ui_.registryNumberEdit->text().trimmed();
      if (registryNumber.length() > model::Resident::registryNumberMaxLen)
      {        
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(registryNumberTitle)).arg(model::Resident::registryNumberMaxLen));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.registryNumberEdit->setFocus(Qt::TabFocusReason);
        ui_.registryNumberEdit->selectAll();
        return false;
      }      
      // Get INTB
      QString intb = ui_.intbEdit->text().trimmed();      
      if (intb.length() > model::Resident::intbMaxLen)
      {        
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(intbTitle)).arg(model::Resident::intbMaxLen));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.intbEdit->setFocus(Qt::TabFocusReason);
        ui_.intbEdit->selectAll();
        return false;
      }      
      // Get CRSA
      QString crsa = ui_.crsaEdit2->text().trimmed();
      if (crsa.length() > model::Resident::crsaMaxLen)
      {        
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(crsaTitle)).arg(model::Resident::crsaMaxLen));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.crsaEdit2->setFocus(Qt::TabFocusReason);
        ui_.crsaEdit2->selectAll();
        return false;
      }      
      if (!selectedCountryId_)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(countryTitle)));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.countryBtn->setFocus(Qt::TabFocusReason);
        return false;
      } 
      // Get region name
      QString regionName = ui_.regionEdit->text().trimmed();
      if (regionName.length() > model::Resident::regionNameMaxLen)
      {        
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(regionNameTitle)).arg(model::Resident::regionNameMaxLen));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.regionEdit->setFocus(Qt::TabFocusReason);
        ui_.regionEdit->selectAll();
        return false;
      }      
      // Get city name
      QString cityName = ui_.cityEdit->text().trimmed();
      if (cityName.length() > model::Resident::cityNameMaxLen)
      {        
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(cityNameTitle)).arg(model::Resident::cityNameMaxLen));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.cityEdit->setFocus(Qt::TabFocusReason);
        ui_.cityEdit->selectAll();
        return false;
      }      
      // Get street name
      QString streetName = ui_.streetEdit->text().trimmed();      
      if (streetName.length() > model::Resident::streetNameMaxLen)
      {        
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(streetNameTitle)).arg(model::Resident::streetNameMaxLen));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.streetEdit->setFocus(Qt::TabFocusReason);
        ui_.streetEdit->selectAll();
        return false;
      }      
      // Get ZIP code
      QString zipCode = ui_.zipEdit->text().trimmed();
      if (zipCode.length() > model::Resident::zipCodeMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(zipCodeTitle)).arg(model::Resident::zipCodeMaxLen));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.zipEdit->setFocus(Qt::TabFocusReason);
        ui_.zipEdit->selectAll();
        return false;
      }      
      // Get house number
      OptionalInt houseNumber;      
      QString houseNumberText(ui_.houseNumberEdit->text().trimmed());
      if (!houseNumberText.isEmpty())
      {
        int number = houseNumberText.toInt(&validated);
        if (validated)
        {
          validated = number >= 0;        
          if (validated)
          {
            houseNumber.reset(number);
          }
        }    
        if (!validated)
        {
          QMessageBox::warning(this, tr(messageTitle), 
            tr(invalidInteger).arg(tr(houseNumberTitle)).arg(0).arg((std::numeric_limits<int>::max)()));
          ui_.tabWidget->setCurrentIndex(contentsTabIndex);
          ui_.houseNumberEdit->setFocus(Qt::TabFocusReason);
          ui_.houseNumberEdit->selectAll();
          return false;
        }
      }      
      // Get advanced house number
      QString advHouseNumber = ui_.advHouseNumberEdit->text().trimmed();
      if (advHouseNumber.length() > model::Resident::advHouseNumberMaxLen)
      {        
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(advHouseNumberTitle)).arg(model::Resident::advHouseNumberMaxLen));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.advHouseNumberEdit->setFocus(Qt::TabFocusReason);
        ui_.advHouseNumberEdit->selectAll();
        return false;
      }      
      // Get case number
      QString caseNumber = ui_.caseNumberEdit->text().trimmed();
      if (caseNumber.length() > model::Resident::caseNumberMaxLen)
      {        
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(caseNumberTitle)).arg(model::Resident::caseNumberMaxLen));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.caseNumberEdit->setFocus(Qt::TabFocusReason);
        ui_.caseNumberEdit->selectAll();
        return false;
      }      
      // Get office number
      OptionalInt officeNumber;
      QString officeNumberText = ui_.officeNumberEdit->text().trimmed();
      if (!officeNumberText.isEmpty())
      {
        int value = officeNumberText.toInt(&validated);
        if (validated)
        {
          validated = value >= 0;        
          if (validated)
          {
            officeNumber.reset(value);
          }
        }
        if (!validated)
        {
          QMessageBox::warning(this, tr(messageTitle), 
            tr(invalidInteger).arg(tr(officeNumberTitle)).arg(0).arg((std::numeric_limits<int>::max)()));
          ui_.tabWidget->setCurrentIndex(contentsTabIndex);
          ui_.officeNumberEdit->setFocus(Qt::TabFocusReason);
          ui_.officeNumberEdit->selectAll();
          return false;
        }  
      }
      // Get advanced office number
      QString advOfficeNumber = ui_.advOfficeNumberEdit->text().trimmed();
      if (advOfficeNumber.length() > model::Resident::advOfficeNumberMaxLen)
      {        
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(advOfficeNumberTitle)).arg(model::Resident::advOfficeNumberMaxLen));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.advOfficeNumberEdit->setFocus(Qt::TabFocusReason);
        ui_.advOfficeNumberEdit->selectAll();
        return false;
      }    
      OptionalQDate sreDate;
      {
        QDate value = ui_.sreDateEdit->date();
        if (!value.isNull())
        {
          sreDate.reset(value);
        }
      }
      
      entityData_.fullName    = fullName;
      entityData_.shortName   = shortName;
      entityData_.countryId   = selectedCountryId_.get();
      entityData_.regionName  = regionName.isEmpty() ? OptionalQString() : regionName;
      entityData_.cityName    = cityName.isEmpty() ? OptionalQString() : cityName;
      entityData_.streetName  = streetName.isEmpty() ? OptionalQString() : streetName;
      entityData_.houseNumber = houseNumber;
      entityData_.advHouseNumber  = advHouseNumber.isEmpty() ? OptionalQString() : advHouseNumber;
      entityData_.caseNumber      = caseNumber.isEmpty() ? OptionalQString() : caseNumber;
      entityData_.officeNumber    = officeNumber;
      entityData_.advOfficeNumber = advOfficeNumber.isEmpty() ? OptionalQString() : advOfficeNumber;
      entityData_.zipCode         = zipCode.isEmpty() ? OptionalQString() : zipCode;
      entityData_.registryNumber  = registryNumber.isEmpty() ? OptionalQString() : registryNumber;
      entityData_.intb            = intb.isEmpty() ? OptionalQString() : intb;
      entityData_.crsa            = crsa.isEmpty() ? OptionalQString() : crsa;      
      entityData_.sreDate         = sreDate;
      return true;
    }
    
    bool ResidentCardPage::populateFromDatabase()
    {       
      static const char* querySql = "select " \
        "t.\"FULL_NAME\", t.\"SHORT_NAME\", t.\"REGISTRY_NUMBER\", t.\"INTB\", t.\"CRSA\", " \
        "t.\"REGION_NAME\", t.\"CITY_NAME\", t.\"STREET_NAME\", " \
        "t.\"HOUSE_NUMBER\", t.\"ADV_HOUSE_NUMBER\", t.\"CASE_NUMBER\", " \
        "t.\"OFFICE_NUMBER\", t.\"ADV_OFFICE_NUMBER\", t.\"ZIP_CODE\", " \
        "t.\"COUNTRY_ID\", cn.\"FULL_NAME\", " \
        "t.\"CREATE_USER_ID\", cu.\"LOGIN\", " \
        "t.\"UPDATE_USER_ID\", uu.\"LOGIN\", " \
        "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", t.\"SRE_DATE\" " \
        "from \"RESIDENT\" t " \
        "join \"COUNTRY\" cn on t.\"COUNTRY_ID\" = cn.\"COUNTRY_ID\" " \
        "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
        "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\""
        "where t.\"RESIDENT_ID\" = ?";

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
              entityData_.residentId = entityId().get();
              entityData_.fullName   = query.value(0).toString();
              entityData_.shortName  = query.value(1).toString();
              entityData_.registryNumber = query.isNull(2) ? OptionalQString() : query.value(2).toString();
              entityData_.intb = query.isNull(3) ? OptionalQString() : query.value(3).toString();
              entityData_.crsa = query.isNull(4) ? OptionalQString() : query.value(4).toString();
              entityData_.regionName = query.isNull(5) ? OptionalQString() : query.value(5).toString();
              entityData_.cityName = query.isNull(6) ? OptionalQString() : query.value(6).toString();
              entityData_.streetName = query.isNull(7) ? OptionalQString() : query.value(7).toString();
              entityData_.houseNumber = query.isNull(8) ? OptionalInt() : query.value(8).toInt();
              entityData_.advHouseNumber = query.isNull(9) ? OptionalQString() : query.value(9).toString();
              entityData_.caseNumber = query.isNull(10) ? OptionalQString() : query.value(10).toString();
              entityData_.officeNumber = query.isNull(11) ? OptionalInt() : query.value(11).toInt();
              entityData_.advOfficeNumber = query.isNull(12) ? OptionalQString() : query.value(12).toString();
              entityData_.zipCode = query.isNull(13) ? OptionalQString() : query.value(13).toString();
              entityData_.countryId = query.value(14).toLongLong();
              countryFullName_ = query.value(15).toString();              
              entityData_.createUserId = query.value(16).toLongLong();
              createUserName_          = query.value(17).toString();
              entityData_.updateUserId = query.value(18).toLongLong();
              updateUserName_          = query.value(19).toString();              
              entityData_.createTime   = databaseModel()->convertFromServer(query.value(20).toDateTime());
              entityData_.updateTime   = databaseModel()->convertFromServer(query.value(21).toDateTime());
              entityData_.sreDate      = query.isNull(22) ? OptionalQDate() : databaseModel()->convertFromServer(query.value(22).toDate());
              return true;
            }
          }
        }
      }
      return false;
    }

    void ResidentCardPage::populateWidgets()
    {
      if (entityId())
      {        
        // Contents page
        ui_.fullNameEdit->setText(entityData_.fullName);
        ui_.shortNameEdit->setText(entityData_.shortName);
        ui_.registryNumberEdit->setText(entityData_.registryNumber ? entityData_.registryNumber.get() : QString());
        ui_.intbEdit->setText(entityData_.intb ? entityData_.intb.get() : QString());
        ui_.crsaEdit2->setText(entityData_.crsa ? entityData_.crsa.get() : QString());
        ui_.regionEdit->setText(entityData_.regionName ? entityData_.regionName.get() : QString());
        ui_.cityEdit->setText(entityData_.cityName ? entityData_.cityName.get() : QString());
        ui_.streetEdit->setText(entityData_.streetName ? entityData_.streetName.get() : QString());
        ui_.zipEdit->setText(entityData_.zipCode ? entityData_.zipCode.get() : QString());
        ui_.houseNumberEdit->setText(entityData_.houseNumber ? QString::number(entityData_.houseNumber.get()) : QString());
        ui_.advHouseNumberEdit->setText(entityData_.advHouseNumber ? entityData_.advHouseNumber.get() : QString());
        ui_.caseNumberEdit->setText(entityData_.caseNumber ? entityData_.caseNumber.get() : QString());
        ui_.officeNumberEdit->setText(entityData_.officeNumber ? QString::number(entityData_.officeNumber.get()) : QString());
        ui_.advOfficeNumberEdit->setText(entityData_.advOfficeNumber ? entityData_.advOfficeNumber.get() : QString());
        if (entityData_.sreDate)
        {
          ui_.sreDateEdit->setDate(entityData_.sreDate.get());
        }
        else 
        {
          ui_.sreDateEdit->clear();
        }        
        // General data page
        ui_.createTimeEdit->setText(entityData_.createTime.toString(Qt::DefaultLocaleShortDate));
        ui_.updateTimeEdit->setText(entityData_.updateTime.toString(Qt::DefaultLocaleShortDate));
        ui_.createUserEdit->setText(createUserName_);
        ui_.updateUserEdit->setText(updateUserName_);
        ui_.countryEdit->setText(countryFullName_);
        selectedCountryId_ = entityData_.countryId;
        populateExportData();
        contentsChanged_ = false;
      }      
    }

    void ResidentCardPage::populateExportData()
    {
      static QString fullNameCaption = tr("Full name");
      static QString shortNameCaption = tr("Short name");
      static QString registryNumberCaption = tr("Registry number");
      static QString intbCaption = tr("INTB");
      static QString crsaCaption = tr("CRSA");
      static QString sreDateCaption = tr("State registry entering date");
      static QString countryFullNameCaption = tr("Country");      
      static QString regionNameCaption = tr("Region");
      static QString cityNameCaption = tr("City");
      static QString streetNameCaption = tr("Street");
      static QString zipCodeCaption = tr("ZIP");
      static QString houseNumberCaption = tr("House number");
      static QString houseNumberAdvCaption = tr("House number, adv");
      static QString houseNumberCaseCaption = tr("House number, case");
      static QString officeNumberCaption = tr("Office number");
      static QString officeNumberAdvCaption = tr("Office number, adv");
      static QString createUserCaption = tr("User, creator");
      static QString createTimeCaption = tr("Time, created");
      static QString updateUserCaption = tr("User, last update");
      static QString updateTimeCaption = tr("Time, last update");                    
        
      ExportDataList exportDataList;
      exportDataList << std::make_pair(fullNameCaption, QVariant(entityData_.fullName));
      exportDataList << std::make_pair(shortNameCaption, QVariant(entityData_.shortName));
      exportDataList << std::make_pair(registryNumberCaption, entityData_.registryNumber ? QVariant(entityData_.registryNumber.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(intbCaption, entityData_.intb ? QVariant(entityData_.intb.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(crsaCaption, entityData_.crsa ? QVariant(entityData_.crsa.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(sreDateCaption, entityData_.sreDate ? QVariant(entityData_.sreDate.get()) : QVariant(QVariant::Date));
      exportDataList << std::make_pair(countryFullNameCaption, QVariant(countryFullName_));      
      exportDataList << std::make_pair(regionNameCaption, entityData_.regionName ? QVariant(entityData_.regionName.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(cityNameCaption, entityData_.cityName ? QVariant(entityData_.cityName.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(streetNameCaption, entityData_.streetName ? QVariant(entityData_.streetName.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(zipCodeCaption, entityData_.zipCode ? QVariant(entityData_.zipCode.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(houseNumberCaption, entityData_.houseNumber? QVariant(entityData_.houseNumber.get()) : QVariant(QVariant::Int));
      if (entityData_.advHouseNumber)
      {
        exportDataList << std::make_pair(houseNumberAdvCaption, QVariant(entityData_.advHouseNumber.get()));
      }      
      if (entityData_.caseNumber)
      {
        exportDataList << std::make_pair(houseNumberCaseCaption, QVariant(entityData_.caseNumber.get()));
      }      
      if (entityData_.officeNumber)
      {
        exportDataList << std::make_pair(officeNumberCaption, QVariant(entityData_.officeNumber.get()));
      }      
      if (entityData_.advOfficeNumber)
      {
        exportDataList << std::make_pair(officeNumberAdvCaption, QVariant(entityData_.advOfficeNumber.get()));
      }
      exportDataList << std::make_pair(createUserCaption, QVariant(createUserName_));
      exportDataList << std::make_pair(createTimeCaption, QVariant(entityData_.createTime));
      exportDataList << std::make_pair(updateUserCaption, QVariant(updateUserName_));
      exportDataList << std::make_pair(updateTimeCaption, QVariant(entityData_.updateTime));      

      setExportDataList(exportDataList);
    }

    bool ResidentCardPage::updateDatabase()
    {       
      static const char* querySql = "update \"RESIDENT\" set " \
        "\"FULL_NAME\" = ?, \"SHORT_NAME\" = ?, \"REGISTRY_NUMBER\" = ?, " \
        "\"INTB\" = ?, \"CRSA\" = ?, \"REGION_NAME\" = ?, \"CITY_NAME\" = ?, \"STREET_NAME\" = ?, " \
        "\"HOUSE_NUMBER\" = ?, \"ADV_HOUSE_NUMBER\" = ?, \"CASE_NUMBER\" = ?, " \
        "\"OFFICE_NUMBER\" = ?, \"ADV_OFFICE_NUMBER\" = ?, \"ZIP_CODE\" = ?, " \
        "\"COUNTRY_ID\" = ?, " \
        "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\', \"SRE_DATE\" = ? " \
        "where \"RESIDENT_ID\" = ?";

      if (entityId())
      {
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.fullName);
          query.bindValue(1, entityData_.shortName);
          query.bindValue(2, entityData_.registryNumber ? entityData_.registryNumber.get() : QVariant(QVariant::String));
          query.bindValue(3, entityData_.intb ? entityData_.intb.get() : QVariant(QVariant::String));          
          query.bindValue(4, entityData_.crsa ? entityData_.crsa.get() : QVariant(QVariant::String));
          query.bindValue(5, entityData_.regionName ? entityData_.regionName.get() : QVariant(QVariant::String));
          query.bindValue(6, entityData_.cityName ? entityData_.cityName.get() : QVariant(QVariant::String));
          query.bindValue(7, entityData_.streetName ? entityData_.streetName.get() : QVariant(QVariant::String));
          query.bindValue(8, entityData_.houseNumber ? entityData_.houseNumber.get() : QVariant(QVariant::Int));
          query.bindValue(9, entityData_.advHouseNumber ? entityData_.advHouseNumber.get() : QVariant(QVariant::String));
          query.bindValue(10, entityData_.caseNumber ? entityData_.caseNumber.get() : QVariant(QVariant::String));
          query.bindValue(11, entityData_.officeNumber ? entityData_.officeNumber.get() : QVariant(QVariant::Int));
          query.bindValue(12, entityData_.advOfficeNumber ? entityData_.advOfficeNumber.get() : QVariant(QVariant::String));
          query.bindValue(13, entityData_.zipCode ? entityData_.zipCode.get() : QVariant(QVariant::String));
          query.bindValue(14, entityData_.countryId);
          query.bindValue(15, databaseModel()->userId());
          query.bindValue(16, entityData_.sreDate ? databaseModel()->convertToServer(entityData_.sreDate.get()) : QVariant(QVariant::Date));
          query.bindValue(17, entityData_.residentId);
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

    bool ResidentCardPage::insertIntoDatabase()
    {
      static const char* querySql = "insert into \"RESIDENT\"(" \
        "\"RESIDENT_ID\", \"FULL_NAME\", \"SHORT_NAME\", \"REGISTRY_NUMBER\", " \
        "\"INTB\", \"CRSA\", \"REGION_NAME\", \"CITY_NAME\", \"STREET_NAME\", " \
        "\"HOUSE_NUMBER\", \"ADV_HOUSE_NUMBER\", \"CASE_NUMBER\", " \
        "\"OFFICE_NUMBER\", \"ADV_OFFICE_NUMBER\", \"ZIP_CODE\", \"COUNTRY_ID\", " \
        "\"CREATE_USER_ID\", \"UPDATE_USER_ID\", \"CREATE_TIME\", \"UPDATE_TIME\", \"SRE_DATE\") " \
        "values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, \'NOW\', \'NOW\', ?)";
     
      if (OptionalQInt64 newEntityId = databaseModel()->generateId(residentEntity))
      {
        entityData_.residentId = newEntityId.get();
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.residentId);
          query.bindValue(1, entityData_.fullName);
          query.bindValue(2, entityData_.shortName);
          query.bindValue(3, entityData_.registryNumber ? entityData_.registryNumber.get() : QVariant(QVariant::String));
          query.bindValue(4, entityData_.intb ? entityData_.intb.get() : QVariant(QVariant::String));          
          query.bindValue(5, entityData_.crsa ? entityData_.crsa.get() : QVariant(QVariant::String));
          query.bindValue(6, entityData_.regionName ? entityData_.regionName.get() : QVariant(QVariant::String));
          query.bindValue(7, entityData_.cityName ? entityData_.cityName.get() : QVariant(QVariant::String));
          query.bindValue(8, entityData_.streetName ? entityData_.streetName.get() : QVariant(QVariant::String));
          query.bindValue(9, entityData_.houseNumber ? entityData_.houseNumber.get() : QVariant(QVariant::Int));
          query.bindValue(10, entityData_.advHouseNumber ? entityData_.advHouseNumber.get() : QVariant(QVariant::String));
          query.bindValue(11, entityData_.caseNumber ? entityData_.caseNumber.get() : QVariant(QVariant::String));
          query.bindValue(12, entityData_.officeNumber ? entityData_.officeNumber.get() : QVariant(QVariant::Int));
          query.bindValue(13, entityData_.advOfficeNumber ? entityData_.advOfficeNumber.get() : QVariant(QVariant::String));
          query.bindValue(14, entityData_.zipCode ? entityData_.zipCode.get() : QVariant(QVariant::String));
          query.bindValue(15, entityData_.countryId);
          query.bindValue(16, databaseModel()->userId());
          query.bindValue(17, databaseModel()->userId());
          query.bindValue(18, entityData_.sreDate ? databaseModel()->convertToServer(entityData_.sreDate.get()) : QVariant(QVariant::Date));
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

    bool ResidentCardPage::removeFromDatabase()
    {   
      static const char* querySql = "delete from \"RESIDENT\" where \"RESIDENT_ID\" = ?";

      if (QMessageBox::Yes == QMessageBox::question(this, tr("Residents"), 
        tr("Delete resident?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
      {
        if (entityId())
        {
          QSqlQuery query(databaseModel()->database());              
          if (query.prepare(querySql))
          {
            query.bindValue(0, entityData_.residentId);
            if (query.exec())
            {
              setEntityId(OptionalQInt64());
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

    void ResidentCardPage::on_lineEdit_textEdited(const QString&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void ResidentCardPage::on_dateEdit_dateChanged(const QDate&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void ResidentCardPage::on_database_userUpdated(const model::User& user)
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

    void ResidentCardPage::on_database_countryUpdated(const model::Country& country)
    {
      if (entityId())
      {
        if (entityData_.countryId == country.countryId)
        {
          countryFullName_ = country.fullName;
        }
      }
      if (selectedCountryId_)
      {        
        if (selectedCountryId_.get() == country.countryId)
        {
          ui_.countryEdit->setText(country.fullName);
          updateWidgets();
        }
      }
    }

    void ResidentCardPage::on_database_countryRemoved(const ma::chrono::model::Country& country)
    {
      if (selectedCountryId_)
      {        
        if (selectedCountryId_.get() == country.countryId)
        {          
          ui_.countryEdit->setText(QString());
          selectedCountryId_.reset();
          updateWidgets();
        }
      }
    }    

    void ResidentCardPage::showRequiredMarkers(bool visible)
    {
      WidgetUtility::show(requiredMarkers_, visible);
    }

    void ResidentCardPage::on_createUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.createUserId);
      }        
    }

    void ResidentCardPage::on_updateUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.updateUserId);
      }        
    }

    void ResidentCardPage::on_countryBtn_clicked(bool /*checked*/)
    {      
      if (viewMode == mode())
      {
        if (entityId())
        {
          showCountryCardTaskWindow(entityData_.countryId);
        }        
      }
      else
      {
        //todo: add default selection
        std::auto_ptr<ListPage> listPage(new CountryListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), 
          UserDefinedFilter(), UserDefinedOrder(), 0));
        ListSelectDialog listSelectDialog(this, listPage);
        listSelectDialog.setWindowModality(Qt::WindowModal);
        if (QDialog::Accepted == listSelectDialog.exec())
        {
          selectedCountryId_ = listSelectDialog.listPage()->selectedId();          
          if (OptionalQString countryFullName = listSelectDialog.listPage()->selectedRelationalText())
          {
            ui_.countryEdit->setText(countryFullName.get());
          }
          contentsChanged_ = true;
          updateWidgets();
        }
      }      
    }

  } // namespace chrono
} // namespace ma
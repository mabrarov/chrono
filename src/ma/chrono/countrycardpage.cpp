/*
TRANSLATOR ma::chrono::CountryCardPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <limits>
#include <QAction>
#include <QMessageBox>
#include <QSqlQuery>
#include <ma/chrono/constants.h>
#include <ma/chrono/dbconstants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/widgetutility.h>
#include <ma/chrono/sqlutility.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/countrynumericcodedelegate.h>
#include <ma/chrono/countrycardpage.h>

namespace ma
{
  namespace chrono
  {    
    CountryCardPage::CountryCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent)
      : CardPage(resourceManager, taskWindowManager, databaseModel, helpAction, countryEntity, mode, entityId, parent)
      , dataAwareWidgets_()
      , contentsChanged_(false)
    {
      ui_.setupUi(this);
      countryNumericCodeDelegate_ = new CountryNumericCodeDelegate(this);
      ui_.tabWidget->setCurrentIndex(0);
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      createAction()->setText(tr("&New country"));
      dataAwareWidgets_ << ui_.fullNameEdit << ui_.shortNameEdit 
                        << ui_.trigramCyrillicCodeEdit << ui_.trigramLatinCodeEdit 
                        << ui_.twoLetterLatinCodeEdit << ui_.numericCodeEdit;
      requiredMarkers_  << ui_.fullNameAsterikLabel << ui_.shortNameAsterikLabel
                        << ui_.trigramCyrillicCodeAsterikLabel << ui_.trigramLatinCodeAsterikLabel
                        << ui_.twoLetterLatinCodeAsterikLabel << ui_.numericCodeAsterikLabel;
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
      updateWidgets();
    }

    CountryCardPage::~CountryCardPage()
    {
    }       

    void CountryCardPage::refresh()
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

    void CountryCardPage::save()
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

    void CountryCardPage::cancel()
    {
      if (editMode == mode())
      {      
        populateWidgets();
      }      
      CardPage::cancel();
      updateWidgets();
    }

    void CountryCardPage::edit()
    {
      CardPage::edit();
      updateWidgets();
    }

    void CountryCardPage::remove()
    {
      if (createMode != mode())
      {
        if (removeFromDatabase())
        {
          CardPage::remove();
        }
      }            
    }

    std::auto_ptr<CardPage> CountryCardPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new CountryCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }

    void CountryCardPage::connectDataAwareWidgets()
    {
      
      typedef QWidgetList::const_iterator const_iterator;
      
      for (const_iterator it = dataAwareWidgets_.begin(), end = dataAwareWidgets_.end(); it != end; ++it)
      {
        QWidget* widget = *it;
        QLineEdit* edit = qobject_cast<QLineEdit*>(widget);
        if (edit)
        {
          QObject::connect(edit, SIGNAL(textEdited(const QString&)), SLOT(on_lineEdit_textEdited(const QString&)));
        }
      }
    }

    void CountryCardPage::updateWidgets()
    {
      
      typedef QWidgetList::const_iterator const_iterator;

      Mode currentMode = mode();
      bool readOnly = viewMode == currentMode;
      for (const_iterator it = dataAwareWidgets_.begin(), end = dataAwareWidgets_.end(); it != end; ++it)
      {
        QWidget* widget = *it;
        QLineEdit* edit = qobject_cast<QLineEdit*>(widget);
        if (edit)
        {
          edit->setReadOnly(readOnly);
          //edit->setEnabled(!readOnly);
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
          updateWindowTitle(tr("Countries - %1").arg(entityData_.fullName));
        }       
        showRequiredMarkers(false);
        break;

      case editMode:
        if (entityId())
        {
          if (contentsChanged_)
          {
            updateWindowTitle(tr("Countries - %1* - edit").arg(entityData_.fullName));
          }
          else
          {
            updateWindowTitle(tr("Countries - %1 - edit").arg(entityData_.fullName));
          }        
        }
        showRequiredMarkers(true);
        break;

      case createMode:
        updateWindowTitle(tr("Countries - New country creation*"));
        showRequiredMarkers(true);
        break;
      }      
    }

    bool CountryCardPage::populateFromWidgets()
    {      
      static const char* fullNameTitle = QT_TR_NOOP("Full name");
      static const char* shortNameTitle = QT_TR_NOOP("Short name");
      static const char* trigramCyrillicCodeTitle = QT_TR_NOOP("Trigram cyrillic code");
      static const char* trigramLatinCodeTitle = QT_TR_NOOP("Trigram latin code");
      static const char* twoLetterLatinCodeTitle = QT_TR_NOOP("Two letter latin code");
      static const char* numericCodeTitle = QT_TR_NOOP("Numeric code");      

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
      else if (fullName.length() > model::Country::fullNameMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(fullNameTitle)).arg(model::Country::fullNameMaxLen));
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
      else if (shortName.length() > model::Country::shortNameMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(shortNameTitle)).arg(model::Country::shortNameMaxLen));                
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
      // Get trigram cyrillic code
      QString trigramCyrillicCode = ui_.trigramCyrillicCodeEdit->text().trimmed();
      if (trigramCyrillicCode.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(trigramCyrillicCodeTitle)));
      } 
      else if (trigramCyrillicCode.length() > model::Country::trigramCyrillicCodeMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(trigramCyrillicCodeTitle)).arg(model::Country::trigramCyrillicCodeMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.trigramCyrillicCodeEdit->setFocus(Qt::TabFocusReason);
        ui_.trigramCyrillicCodeEdit->selectAll();
        return false;
      }
      // Get trigram latin code
      QString trigramLatinCode = ui_.trigramLatinCodeEdit->text().trimmed();;      
      if (trigramLatinCode.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(trigramLatinCodeTitle)));
      } 
      else if (trigramLatinCode.length() > model::Country::trigramLatinCodeMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(trigramLatinCodeTitle)).arg(model::Country::trigramLatinCodeMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.trigramLatinCodeEdit->setFocus(Qt::TabFocusReason);
        ui_.trigramLatinCodeEdit->selectAll();
        return false;
      }
      // Get two letter latin code
      QString twoLetterLatinCode = ui_.twoLetterLatinCodeEdit->text().trimmed();      
      if (twoLetterLatinCode.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(twoLetterLatinCodeTitle)));
      } 
      else if (twoLetterLatinCode.length() > model::Country::twoLetterLatinCodeMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(twoLetterLatinCodeTitle)).arg(model::Country::twoLetterLatinCodeMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.twoLetterLatinCodeEdit->setFocus(Qt::TabFocusReason);
        ui_.twoLetterLatinCodeEdit->selectAll();
        return false;
      }
      // Get numeric code
      int numericCode = ui_.numericCodeEdit->text().trimmed().toInt(&validated);
      if (validated)
      {
        validated = numericCode >= 0;        
      }
      if (!validated)
      {
        QMessageBox::warning(this, tr(messageTitle), 
          tr(invalidInteger).arg(tr(numericCodeTitle)).arg(0).arg((std::numeric_limits<int>::max)()));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.numericCodeEdit->setFocus(Qt::TabFocusReason);
        ui_.numericCodeEdit->selectAll();
        return false;
      }
      entityData_.fullName            = fullName;
      entityData_.shortName           = shortName;
      entityData_.trigramCyrillicCode = trigramCyrillicCode;
      entityData_.trigramLatinCode    = trigramLatinCode;
      entityData_.twoLetterLatinCode  = twoLetterLatinCode;
      entityData_.numericCode         = numericCode; 
      return true;
    }

    bool CountryCardPage::populateFromDatabase()
    {
      static const char* querySql = "select " \
        "c.\"FULL_NAME\", c.\"SHORT_NAME\", " \
        "c.\"TRIGRAM_CYRILLIC_CODE\", c.\"TRIGRAM_LATIN_CODE\", " \
        "c.\"TWO_LETTER_LATIN_CODE\", c.\"NUMERIC_CODE\", " \
        "c.\"CREATE_USER_ID\", cu.\"LOGIN\", " \
        "c.\"UPDATE_USER_ID\", uu.\"LOGIN\", " \
        "c.\"CREATE_TIME\", c.\"UPDATE_TIME\" " \
        "from \"COUNTRY\" c " \
        "join \"USER\" cu on c.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
        "join \"USER\" uu on c.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
        "where c.\"COUNTRY_ID\" = ?";

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
              entityData_.countryId = entityId().get();
              entityData_.fullName  = query.value(0).toString();
              entityData_.shortName = query.value(1).toString();
              entityData_.trigramCyrillicCode = query.value(2).toString();
              entityData_.trigramLatinCode    = query.value(3).toString();
              entityData_.twoLetterLatinCode  = query.value(4).toString();
              entityData_.numericCode  = query.value(5).toInt();
              entityData_.createUserId = query.value(6).toLongLong();
              createUserName_          = query.value(7).toString();
              entityData_.updateUserId = query.value(8).toLongLong();
              updateUserName_          = query.value(9).toString();              
              entityData_.createTime   = databaseModel()->convertFromServer(query.value(10).toDateTime());
              entityData_.updateTime   = databaseModel()->convertFromServer(query.value(11).toDateTime());
              return true;
            }
          }
        }
      }
      return false;
    }

    void CountryCardPage::populateWidgets()
    {
      if (entityId())
      {      
        // Contents page
        ui_.fullNameEdit->setText(entityData_.fullName);
        ui_.shortNameEdit->setText(entityData_.shortName);
        ui_.trigramCyrillicCodeEdit->setText(entityData_.trigramCyrillicCode);
        ui_.trigramLatinCodeEdit->setText(entityData_.trigramLatinCode);
        ui_.twoLetterLatinCodeEdit->setText(entityData_.twoLetterLatinCode);
        ui_.numericCodeEdit->setText(countryNumericCodeDelegate_->displayText(entityData_.numericCode, QLocale()));
        // General data page
        ui_.createTimeEdit->setText(entityData_.createTime.toString(Qt::DefaultLocaleShortDate));
        ui_.updateTimeEdit->setText(entityData_.updateTime.toString(Qt::DefaultLocaleShortDate));
        ui_.createUserEdit->setText(createUserName_);
        ui_.updateUserEdit->setText(updateUserName_);        
        populateExportData();
        contentsChanged_ = false;
      }
    } 

    void CountryCardPage::populateExportData()
    {      
      static const QString fullNameCaption = tr("Full name");      
      static const QString shortNameCaption = tr("Short name");
      static const QString trigramCyrCodeCaption = tr("Trigram cyrillic code");
      static const QString trigramLatinCodeCaption = tr("Trigram latin code");      
      static const QString twoLetterLatinCodeCaption = tr("Two letter latin code");      
      static const QString numericCodeCaption = tr("Numeric code");
      static const QString createUserCaption = tr("User, creator");
      static const QString createTimeCaption = tr("Time, created");
      static const QString updateUserCaption = tr("User, last update");
      static const QString updateTimeCaption = tr("Time, last update");
        
      ExportDataList exportDataList;
      exportDataList << std::make_pair(fullNameCaption, QVariant(entityData_.fullName));      
      exportDataList << std::make_pair(shortNameCaption, QVariant(entityData_.shortName));
      exportDataList << std::make_pair(trigramCyrCodeCaption, QVariant(entityData_.trigramCyrillicCode));
      exportDataList << std::make_pair(trigramLatinCodeCaption, QVariant(entityData_.trigramLatinCode));
      exportDataList << std::make_pair(twoLetterLatinCodeCaption, QVariant(entityData_.twoLetterLatinCode));
      exportDataList << std::make_pair(numericCodeCaption, QVariant(entityData_.numericCode));
      exportDataList << std::make_pair(createUserCaption, QVariant(createUserName_));
      exportDataList << std::make_pair(createTimeCaption, QVariant(entityData_.createTime));
      exportDataList << std::make_pair(updateUserCaption, QVariant(updateUserName_));
      exportDataList << std::make_pair(updateTimeCaption, QVariant(entityData_.updateTime));      
            
      setExportDataList(exportDataList);
    }

    bool CountryCardPage::updateDatabase()
    {
      static const char* querySql = "update \"COUNTRY\" set " \
        "\"FULL_NAME\" = ?, \"SHORT_NAME\" = ?, \"TRIGRAM_CYRILLIC_CODE\" = ?, " \
        "\"TRIGRAM_LATIN_CODE\" = ?, \"TWO_LETTER_LATIN_CODE\" = ?, " \
        "\"NUMERIC_CODE\" = ?, \"UPDATE_USER_ID\" = ?, " \
        "\"UPDATE_TIME\" = \'NOW\' " \
        "where \"COUNTRY_ID\" = ?";

      if (entityId())
      {
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.fullName);
          query.bindValue(1, entityData_.shortName);
          query.bindValue(2, entityData_.trigramCyrillicCode);
          query.bindValue(3, entityData_.trigramLatinCode);
          query.bindValue(4, entityData_.twoLetterLatinCode);
          query.bindValue(5, entityData_.numericCode);
          query.bindValue(6, databaseModel()->userId());
          query.bindValue(7, entityData_.countryId);
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

    bool CountryCardPage::insertIntoDatabase()
    {
      static const char* querySql = "insert into \"COUNTRY\"(" \
        "\"COUNTRY_ID\", \"FULL_NAME\", \"SHORT_NAME\", \"TRIGRAM_CYRILLIC_CODE\", " \
        "\"TRIGRAM_LATIN_CODE\", \"TWO_LETTER_LATIN_CODE\", \"NUMERIC_CODE\", " \
        "\"CREATE_USER_ID\", \"UPDATE_USER_ID\", \"CREATE_TIME\", \"UPDATE_TIME\") " \
        "values (?, ?, ?, ?, ?, ?, ?, ?, ?, \'NOW\', \'NOW\')";
      
      if (OptionalQInt64 newEntityId = databaseModel()->generateId(countryEntity))
      {
        entityData_.countryId = newEntityId.get();
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.countryId);
          query.bindValue(1, entityData_.fullName);
          query.bindValue(2, entityData_.shortName);
          query.bindValue(3, entityData_.trigramCyrillicCode);
          query.bindValue(4, entityData_.trigramLatinCode);
          query.bindValue(5, entityData_.twoLetterLatinCode);
          query.bindValue(6, entityData_.numericCode);
          query.bindValue(7, databaseModel()->userId());
          query.bindValue(8, databaseModel()->userId());
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

    bool CountryCardPage::removeFromDatabase()
    {
      static const char* querySql = "delete from \"COUNTRY\" where \"COUNTRY_ID\" = ?";

      if (QMessageBox::Yes == QMessageBox::question(this, tr("Countries"), 
        tr("Delete country?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
      {
        if (entityId())
        {
          QSqlQuery query(databaseModel()->database());              
          if (query.prepare(querySql))
          {
            query.bindValue(0, entityData_.countryId);
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

    void CountryCardPage::on_lineEdit_textEdited(const QString&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void CountryCardPage::on_database_userUpdated(const model::User& user)
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

    void CountryCardPage::showRequiredMarkers(bool visible)
    {
      WidgetUtility::show(requiredMarkers_, visible);
    }

    void CountryCardPage::on_createUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.createUserId);
      }        
    }

    void CountryCardPage::on_updateUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.updateUserId);
      }        
    }

  } // namespace chrono
} //namespace ma
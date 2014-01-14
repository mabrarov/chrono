/*
TRANSLATOR ma::chrono::CurrencyCardPage
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
#include <QSqlQuery>
#include <ma/chrono/constants.h>
#include <ma/chrono/dbconstants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/widgetutility.h>
#include <ma/chrono/sqlutility.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/currencynumericcodedelegate.h>
#include <ma/chrono/currencycardpage.h>

namespace ma
{
  namespace chrono
  {    
    CurrencyCardPage::CurrencyCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent)
      : CardPage(resourceManager, taskWindowManager, databaseModel, helpAction, currencyEntity, mode, entityId, parent)
      , dataAwareWidgets_()
      , contentsChanged_(false)
    {
      ui_.setupUi(this);      
      currencyNumericCodeDelegate_ = new CurrencyNumericCodeDelegate(this);

      ui_.tabWidget->setCurrentIndex(0);
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      createAction()->setText(tr("&New currency"));
      dataAwareWidgets_ << ui_.fullNameEdit << ui_.alphabeticCodeEdit << ui_.numericCodeEdit;
      requiredMarkers_  << ui_.fullNameAsterikLabel 
                        << ui_.alphabeticCodeAsterikLabel 
                        << ui_.numericCodeAsterikLabel;
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

    CurrencyCardPage::~CurrencyCardPage()
    {
    }

    void CurrencyCardPage::refresh()
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

    void CurrencyCardPage::save()
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

    void CurrencyCardPage::cancel()
    {
      if (editMode == mode())
      {      
        populateWidgets();
      }      
      CardPage::cancel();
      updateWidgets();
    }

    void CurrencyCardPage::edit()
    {      
      CardPage::edit();
      updateWidgets();
    }

    void CurrencyCardPage::remove()
    {
      if (createMode != mode())
      {
        if (removeFromDatabase())
        {
          CardPage::remove();
        }
      }            
    }

    std::auto_ptr<CardPage> CurrencyCardPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new CurrencyCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }

    void CurrencyCardPage::connectDataAwareWidgets()
    {
      
      typedef QWidgetList::const_iterator const_iterator;
      
      for (const_iterator it = dataAwareWidgets_.begin(), end = dataAwareWidgets_.end(); it != end; ++it)
      {
        QWidget* widget = *it;        
        if (QLineEdit* edit = qobject_cast<QLineEdit*>(widget))
        {
          QObject::connect(edit, SIGNAL(textEdited(const QString&)), SLOT(on_lineEdit_textEdited(const QString&)));
        }        
      }
    }

    void CurrencyCardPage::updateWidgets()
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
      }
      bool peristance = createMode != currentMode && entityId();      
      int generalTabIndex = ui_.tabWidget->indexOf(ui_.generalTab);      
      ui_.tabWidget->setTabEnabled(generalTabIndex, peristance);            
      switch (currentMode)
      {
      case viewMode:
        if (entityId())
        {
          updateWindowTitle(tr("Currency - %1").arg(entityData_.fullName));
        }
        showRequiredMarkers(false);
        break;

      case editMode:
        if (entityId())
        {
          if (contentsChanged_)
          {
            updateWindowTitle(tr("Currency - %1* - edit").arg(entityData_.fullName));
          }
          else
          {
            updateWindowTitle(tr("Currency - %1 - edit").arg(entityData_.fullName));
          }        
        }
        showRequiredMarkers(true);
        break;

      case createMode:
        updateWindowTitle(tr("Currency - New currency creation*"));
        showRequiredMarkers(true);
        break;
      }      
    }

    bool CurrencyCardPage::populateFromWidgets()
    {      
      static const char* fullNameTitle = QT_TR_NOOP("Full name");
      static const char* alphabeticCodeTitle = QT_TR_NOOP("Alphabetic code");      
      static const char* numericCodeTitle = QT_TR_NOOP("Numeric code");
      static const char* fracDigitsTitle = QT_TR_NOOP("Fractional digits");

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
      else if (fullName.length() > model::Currency::fullNameMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(fullNameTitle)).arg(model::Currency::fullNameMaxLen));
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
      // Get alphabetic code
      QString alphabeticCode = ui_.alphabeticCodeEdit->text().trimmed();      
      if (alphabeticCode.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(alphabeticCodeTitle)));
      } 
      else if (alphabeticCode.length() > model::Currency::alphabeticCodeMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(alphabeticCodeTitle)).arg(model::Currency::alphabeticCodeMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.alphabeticCodeEdit->setFocus(Qt::TabFocusReason);
        ui_.alphabeticCodeEdit->selectAll();
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
      entityData_.fullName       = fullName;
      entityData_.alphabeticCode = alphabeticCode;
      entityData_.numericCode    = numericCode;      
      return true;
    }
    
    bool CurrencyCardPage::populateFromDatabase()
    {      
      static const char* querySql = "select " \
        "t.\"FULL_NAME\", t.\"ALPHABETIC_CODE\", t.\"NUMERIC_CODE\", " \
        "t.\"CREATE_USER_ID\", cu.\"LOGIN\", " \
        "t.\"UPDATE_USER_ID\", uu.\"LOGIN\", " \
        "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
        "from \"CURRENCY\" t " \
        "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
        "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
        "where t.\"CURRENCY_ID\" = ?";

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
              entityData_.currencyId = entityId().get();
              entityData_.fullName   = query.value(0).toString();
              entityData_.alphabeticCode = query.value(1).toString();
              entityData_.numericCode  = query.value(2).toInt();
              entityData_.createUserId = query.value(3).toLongLong();
              createUserName_          = query.value(4).toString();
              entityData_.updateUserId = query.value(5).toLongLong();
              updateUserName_          = query.value(6).toString();              
              entityData_.createTime   = databaseModel()->convertFromServer(query.value(7).toDateTime());
              entityData_.updateTime   = databaseModel()->convertFromServer(query.value(8).toDateTime());
              return true;
            }
          }
        }
      }
      return false;
    }

    void CurrencyCardPage::populateWidgets()
    {
      if (entityId())
      {
        // Contents page
        ui_.fullNameEdit->setText(entityData_.fullName);
        ui_.alphabeticCodeEdit->setText(entityData_.alphabeticCode);
        ui_.numericCodeEdit->setText(currencyNumericCodeDelegate_->displayText(entityData_.numericCode, QLocale()));
        // General data page
        ui_.createTimeEdit->setText(entityData_.createTime.toString(Qt::DefaultLocaleShortDate));
        ui_.updateTimeEdit->setText(entityData_.updateTime.toString(Qt::DefaultLocaleShortDate));
        ui_.createUserEdit->setText(createUserName_);
        ui_.updateUserEdit->setText(updateUserName_);
        populateExportData();
        contentsChanged_ = false;
      }      
    }    

    void CurrencyCardPage::populateExportData()
    {      
      static const QString fullNameCaption = tr("Full name");      
      static const QString alphabeticCodeCaption = tr("Alphabetic code");
      static const QString numericCodeCaption = tr("Numeric code");      
      static const QString createUserCaption = tr("User, creator");
      static const QString createTimeCaption = tr("Time, created");
      static const QString updateUserCaption = tr("User, last update");
      static const QString updateTimeCaption = tr("Time, last update");
        
      ExportDataList exportDataList;
      exportDataList << std::make_pair(fullNameCaption, QVariant(entityData_.fullName));      
      exportDataList << std::make_pair(alphabeticCodeCaption, QVariant(entityData_.alphabeticCode));
      exportDataList << std::make_pair(numericCodeCaption, QVariant(entityData_.numericCode));           
      exportDataList << std::make_pair(createUserCaption, QVariant(createUserName_));
      exportDataList << std::make_pair(createTimeCaption, QVariant(entityData_.createTime));
      exportDataList << std::make_pair(updateUserCaption, QVariant(updateUserName_));
      exportDataList << std::make_pair(updateTimeCaption, QVariant(entityData_.updateTime));      
            
      setExportDataList(exportDataList);
    }

    bool CurrencyCardPage::updateDatabase()
    {      
      static const char* querySql = "update \"CURRENCY\" set " \
        "\"FULL_NAME\" = ?, \"ALPHABETIC_CODE\" = ?, \"NUMERIC_CODE\" = ?, " \
        "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\' " \
        "where \"CURRENCY_ID\" = ?";

      if (entityId())
      {
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.fullName);
          query.bindValue(1, entityData_.alphabeticCode);
          query.bindValue(2, entityData_.numericCode);          
          query.bindValue(3, databaseModel()->userId());          
          query.bindValue(4, entityData_.currencyId);
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

    bool CurrencyCardPage::insertIntoDatabase()
    {      
      static const char* querySql = "insert into \"CURRENCY\"(" \
        "\"CURRENCY_ID\", \"FULL_NAME\", \"ALPHABETIC_CODE\", \"NUMERIC_CODE\", " \
        "\"CREATE_USER_ID\", \"UPDATE_USER_ID\", \"CREATE_TIME\", \"UPDATE_TIME\") " \
        "values (?, ?, ?, ?, ?, ?, \'NOW\', \'NOW\')";
      
      if (OptionalQInt64 newEntityId = databaseModel()->generateId(currencyEntity))
      {
        entityData_.currencyId = newEntityId.get();
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.currencyId);
          query.bindValue(1, entityData_.fullName);
          query.bindValue(2, entityData_.alphabeticCode);
          query.bindValue(3, entityData_.numericCode);                    
          query.bindValue(4, databaseModel()->userId());
          query.bindValue(5, databaseModel()->userId());          
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

    bool CurrencyCardPage::removeFromDatabase()
    {      
      static const char* querySql = "delete from \"CURRENCY\" where \"CURRENCY_ID\" = ?";

      if (QMessageBox::Yes == QMessageBox::question(this, tr("Currency"), 
        tr("Delete currency?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
      {
        if (entityId())
        {
          QSqlQuery query(databaseModel()->database());              
          if (query.prepare(querySql))
          {
            query.bindValue(0, entityData_.currencyId);
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

    void CurrencyCardPage::on_lineEdit_textEdited(const QString&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void CurrencyCardPage::on_database_userUpdated(const model::User& user)
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

    void CurrencyCardPage::showRequiredMarkers(bool visible)
    {
      WidgetUtility::show(requiredMarkers_, visible);
    }

    void CurrencyCardPage::on_createUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.createUserId);
      }        
    }

    void CurrencyCardPage::on_updateUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.updateUserId);
      }        
    }    

  } // namespace chrono
} // namespace ma
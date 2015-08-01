/*
TRANSLATOR ma::chrono::UserCardPage
*/

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <QAction>
#include <QMessageBox>
#include <QSqlQuery>
#include <ma/chrono/constants.h>
#include <ma/chrono/dbconstants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/widgetutility.h>
#include <ma/chrono/sqlutility.h>
#include <ma/chrono/usercardpage.h>

namespace ma
{
  namespace chrono
  {    
    UserCardPage::UserCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent)
      : CardPage(resourceManager, taskWindowManager, databaseModel, helpAction, userEntity, mode, entityId, parent)
      , dataAwareWidgets_()
      , changePasswordSelectWidgets_()
      , passwordAwareWidgets_()
      , contentsChanged_(false)
    {
      ui_.setupUi(this);
      ui_.tabWidget->setCurrentIndex(0);
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      createAction()->setText(tr("&New user"));
      dataAwareWidgets_ << ui_.userNameEdit << ui_.firstNameEdit 
                        << ui_.lastNameEdit << ui_.middleNameEdit 
                        << ui_.passwordEdit << ui_.changePasswordCheck;
      requiredMarkers_  << ui_.userNameAsterikLabel 
                        << ui_.firstNameAsterikLabel 
                        << ui_.lastNameAsterikLabel;
      passwordAwareWidgets_ << ui_.passwordLabel << ui_.passwordEdit;
      changePasswordSelectWidgets_ << ui_.changePasswordLabel << ui_.changePasswordCheck;            
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

    UserCardPage::~UserCardPage()
    {
    }       

    void UserCardPage::refresh()
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

    void UserCardPage::save()
    {
      switch (mode())
      {
      case editMode:
        if (checkAccess())
        {          
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

    void UserCardPage::cancel()
    {
      if (editMode == mode())
      {      
        populateWidgets();
      }      
      CardPage::cancel();
      updateWidgets();
    }

    void UserCardPage::edit()
    {
      ui_.changePasswordCheck->setCheckState(Qt::Unchecked);
      ui_.passwordEdit->setText(QString());
      CardPage::edit();
      updateWidgets();
    }

    void UserCardPage::remove()
    {
      if (createMode != mode())
      {
        if (checkAccess(false))
        {
          if (removeFromDatabase())
          {
            CardPage::remove();
          }
        }        
      }            
    }

    std::auto_ptr<CardPage> UserCardPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new UserCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }

    void UserCardPage::connectDataAwareWidgets()
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

    void UserCardPage::updateWidgets()
    {
      
      typedef QWidgetList::const_iterator const_iterator;

      Mode currentMode = mode();
      bool readOnly = viewMode == currentMode;
      WidgetUtility::setReadOnly(dataAwareWidgets_, readOnly);

      bool canSelectPasswordChange = !readOnly && editMode == currentMode;
      for (const_iterator it = changePasswordSelectWidgets_.begin(), end = changePasswordSelectWidgets_.end(); it != end; ++it)
      {
        QWidget* widget = *it;
        widget->setVisible(canSelectPasswordChange);
      }
      bool wantChangePassword = !readOnly && (createMode == currentMode || ui_.changePasswordCheck->checkState() == Qt::Checked);
      for (const_iterator it = passwordAwareWidgets_.begin(), end = passwordAwareWidgets_.end(); it != end; ++it)
      {
        QWidget* widget = *it;
        widget->setVisible(wantChangePassword);
      }
      bool peristance = createMode != currentMode && entityId();      
      int generalTabIndex = ui_.tabWidget->indexOf(ui_.generalTab);      
      ui_.tabWidget->setTabEnabled(generalTabIndex, peristance);            
      switch (currentMode)
      {
      case viewMode:
        if (entityId())
        {
          updateWindowTitle(tr("Users - %1").arg(entityData_.userName));
        } 
        showRequiredMarkers(false);
        break;

      case editMode:
        if (entityId())
        {
          if (contentsChanged_)
          {
            updateWindowTitle(tr("Users - %1* - edit").arg(entityData_.userName));
          }
          else
          {
            updateWindowTitle(tr("Users - %1 - edit").arg(entityData_.userName));
          }        
        }
        showRequiredMarkers(true);
        break;

      case createMode:
        updateWindowTitle(tr("Users - New user creation*"));
        showRequiredMarkers(true);
        break;
      }      
    }

    bool UserCardPage::populateFromWidgets()
    {
      static const char* userNameTitle   = QT_TR_NOOP("User name");
      static const char* firstNameTitle  = QT_TR_NOOP("First name");
      static const char* lastNameTitle    = QT_TR_NOOP("Last name");
      static const char* middleNameTitle = QT_TR_NOOP("Middle name");
      static const char* passwordTitle   = QT_TR_NOOP("Password");      

      static const char* messageTitle = QT_TR_NOOP("Invalid data input");
      static const char* notEmpty     = QT_TR_NOOP("Field \"%1\" can\'t be empty.");
      static const char* tooLongText  = QT_TR_NOOP("Field \"%1\" can\'t exceed %2 characters.");      

      int contentsTabIndex = ui_.tabWidget->indexOf(ui_.contentsTab);
      bool validated;
      // Get user name
      QString userName = ui_.userNameEdit->text().trimmed();      
      if (userName.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(userNameTitle)));
      } 
      else if (userName.length() > model::User::userNameMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(userNameTitle)).arg(model::User::userNameMaxLen));                
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.userNameEdit->setFocus(Qt::TabFocusReason);
        ui_.userNameEdit->selectAll();
        return false;
      }
      // Get first name
      QString firstName = ui_.firstNameEdit->text().trimmed();      
      if (firstName.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(firstNameTitle)));
      } 
      else if (firstName.length() > model::User::firstNameMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(firstNameTitle)).arg(model::User::firstNameMaxLen));                
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.firstNameEdit->setFocus(Qt::TabFocusReason);
        ui_.firstNameEdit->selectAll();
        return false;
      }
      // Get lastName
      QString lastName = ui_.lastNameEdit->text().trimmed();
      if (lastName.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(lastNameTitle)));
      } 
      else if (lastName.length() > model::User::lastNameMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(lastNameTitle)).arg(model::User::lastNameMaxLen));                
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.lastNameEdit->setFocus(Qt::TabFocusReason);
        ui_.lastNameEdit->selectAll();
        return false;
      }
      // Get second name
      QString middleName = ui_.middleNameEdit->text().trimmed();      
      if (middleName.length() > model::User::middleNameMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(middleNameTitle)).arg(model::User::middleNameMaxLen));                
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.middleNameEdit->setFocus(Qt::TabFocusReason);
        ui_.middleNameEdit->selectAll();
        return false;
      }
      Mode currentMode = mode();
      OptionalQString passwordHash;
      if (createMode == currentMode || (editMode == currentMode && ui_.changePasswordCheck->checkState() == Qt::Checked))
      {
        // Get password
        QString password = ui_.passwordEdit->text();
        if (password.isEmpty())
        {
          validated = false;
          QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(passwordTitle)));
        } 
        else if (password.length() > model::User::passwordMaxLen)
        {
          validated = false;
          QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(passwordTitle)).arg(model::User::passwordMaxLen));
        }
        else
        {
          validated = true;
        }        
        if (!validated)
        {
          ui_.tabWidget->setCurrentIndex(contentsTabIndex);
          ui_.passwordEdit->setFocus(Qt::TabFocusReason);
          ui_.passwordEdit->selectAll();          
        }
        if (validated)
        {
          ui_.passwordEdit->setText(QString());
          //todo: add password hashing
          passwordHash.reset(password);
        }        
        password.fill('*');
        (void) password;
        if (!validated)
        {
          return false;
        }
      }
      entityData_.userName   = userName;
      entityData_.firstName  = firstName;
      entityData_.lastName    = lastName;
      entityData_.middleName = middleName.isEmpty() ? OptionalQString() : middleName;
      passwordHash_          = passwordHash;
      return true;
    }

    bool UserCardPage::populateFromDatabase()
    {
      static const char* querySql = "select " \
        "u.\"LOGIN\", u.\"SURNAME\", " \
        "u.\"FIRST_NAME\", u.\"SECOND_NAME\", " \
        "u.\"CREATE_USER_ID\", cu.\"LOGIN\", " \
        "u.\"UPDATE_USER_ID\", uu.\"LOGIN\", " \
        "u.\"CREATE_TIME\", u.\"UPDATE_TIME\" " \
        "from \"USER\" u " \
        "left join \"USER\" cu on u.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
        "left join \"USER\" uu on u.\"UPDATE_USER_ID\" = uu.\"USER_ID\"" \
        "where u.\"USER_ID\" = ?";

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
              entityData_.userId    = entityId().get();
              entityData_.userName  = query.value(0).toString();
              entityData_.lastName   = query.value(1).toString();
              entityData_.firstName = query.value(2).toString();              
              if (query.isNull(3))
              {
                entityData_.middleName.reset();
              }
              else
              {
                entityData_.middleName = query.value(3).toString();
              }
              if (query.isNull(4))
              {
                entityData_.createUserId.reset();
              }
              else
              {
                entityData_.createUserId = query.value(4).toLongLong();
              }
              if (query.isNull(5))
              {
                createUserName_.reset();
              }
              else
              {
                createUserName_ = query.value(5).toString();
              }  
              if (query.isNull(6))
              {
                entityData_.updateUserId.reset();
              }
              else
              {
                entityData_.updateUserId = query.value(6).toLongLong();
              }
              if (query.isNull(7))
              {
                updateUserName_.reset();
              }
              else
              {
                updateUserName_ = query.value(7).toString();
              }                            
              entityData_.createTime = databaseModel()->convertFromServer(query.value(8).toDateTime());
              entityData_.updateTime = databaseModel()->convertFromServer(query.value(9).toDateTime());              
              passwordHash_.reset();
              return true;
            }
          }
        }
      }
      return false;
    }

    void UserCardPage::populateWidgets()
    {
      if (entityId())
      {      
        // Contents page
        ui_.userNameEdit->setText(entityData_.userName);
        ui_.firstNameEdit->setText(entityData_.firstName);
        ui_.lastNameEdit->setText(entityData_.lastName);
        if (entityData_.middleName)
        {
          ui_.middleNameEdit->setText(entityData_.middleName.get());
        }
        else
        {
          ui_.middleNameEdit->setText(QString());
        }              
        // General data page
        ui_.createTimeEdit->setText(entityData_.createTime.toString(Qt::DefaultLocaleShortDate));
        ui_.updateTimeEdit->setText(entityData_.updateTime.toString(Qt::DefaultLocaleShortDate));
        if (createUserName_)
        {
          ui_.createUserEdit->setText(createUserName_.get());
          ui_.createUserEdit->setEnabled(true);
        }
        else
        {
          ui_.createUserEdit->setText(QString());
          ui_.createUserEdit->setEnabled(false);
        }
        ui_.createUserBtn->setEnabled(static_cast<bool>(entityData_.createUserId));
        if (updateUserName_)
        {
          ui_.updateUserEdit->setText(updateUserName_.get());
          ui_.updateUserEdit->setEnabled(true);
        }
        else
        {
          ui_.updateUserEdit->setText(QString());
          ui_.updateUserEdit->setEnabled(false);
        }
        ui_.updateUserBtn->setEnabled(static_cast<bool>(entityData_.updateUserId));
        populateExportData();
        contentsChanged_ = false;
      }
    }

    void UserCardPage::populateExportData()
    {
      static QString userNumberCaption = tr("User");
      static QString firstNameCaption = tr("First name");
      static QString lastNameCaption = tr("Last name");
      static QString middleNameCaption = tr("Middle name");
      static QString createUserCaption = tr("User, creator");
      static QString createTimeCaption = tr("Time, created");
      static QString updateUserCaption = tr("User, last update");
      static QString updateTimeCaption = tr("Time, last update");                    
        
      ExportDataList exportDataList;      
      exportDataList << std::make_pair(userNumberCaption, QVariant(entityData_.userName));
      exportDataList << std::make_pair(firstNameCaption, QVariant(entityData_.firstName));
      exportDataList << std::make_pair(lastNameCaption, QVariant(entityData_.lastName));
      exportDataList << std::make_pair(middleNameCaption, entityData_.middleName ? QVariant(entityData_.middleName.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(createUserCaption, createUserName_ ? QVariant(createUserName_.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(createTimeCaption, QVariant(entityData_.createTime));
      exportDataList << std::make_pair(updateUserCaption, updateUserName_ ? QVariant(updateUserName_.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(updateTimeCaption, QVariant(entityData_.updateTime));      

      setExportDataList(exportDataList);
    }

    bool UserCardPage::updateDatabase()
    {
      static const char* querySql = "update \"USER\" set " \
        "\"LOGIN\" = ?, \"FIRST_NAME\" = ?, \"SURNAME\" = ?, \"SECOND_NAME\" = ?, " \
        "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\' " \
        "where \"USER_ID\" = ?";

      static const char* querySql2 = "update \"USER\" set " \
        "\"LOGIN\" = ?, \"FIRST_NAME\" = ?, \"SURNAME\" = ?, \"SECOND_NAME\" = ?, " \
        "\"PASS_HASH\" = ?, " \
        "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\' " \
        "where \"USER_ID\" = ?";

      if (entityId())
      {        
        QSqlQuery query(databaseModel()->database());
        if (passwordHash_)
        {
          if (!query.prepare(querySql2))
          {
            return false;
          }
          query.bindValue(0, entityData_.userName);
          query.bindValue(1, entityData_.firstName);
          query.bindValue(2, entityData_.lastName);
          query.bindValue(3, entityData_.middleName ? entityData_.middleName.get() : QVariant(QVariant::String));
          query.bindValue(4, passwordHash_.get());
          query.bindValue(5, databaseModel()->userId());            
          query.bindValue(6, entityData_.userId);
        }
        else
        {
          if (!query.prepare(querySql))
          {
            return false;
          }
          query.bindValue(0, entityData_.userName);
          query.bindValue(1, entityData_.firstName);
          query.bindValue(2, entityData_.lastName);
          query.bindValue(3, entityData_.middleName ? entityData_.middleName.get() : QVariant(QVariant::String));
          query.bindValue(4, databaseModel()->userId());            
          query.bindValue(5, entityData_.userId);
        }                    
        if (query.exec())
        {
          if (0 < query.numRowsAffected())
          {
            databaseModel()->notifyUpdate(entityData_);
            return true;
          }            
        }
      }
      return false;
    }

    bool UserCardPage::insertIntoDatabase()
    {
      static const char* querySql = "insert into \"USER\"(" \
        "\"USER_ID\", \"LOGIN\", \"FIRST_NAME\", \"SURNAME\", \"SECOND_NAME\", \"PASS_HASH\", " \
        "\"CREATE_USER_ID\", \"UPDATE_USER_ID\", \"CREATE_TIME\", \"UPDATE_TIME\") " \
        "values (?, ?, ?, ?, ?, ?, ?, ?, \'NOW\', \'NOW\')";
      
      if (passwordHash_)
      {      
        if (OptionalQInt64 newEntityId = databaseModel()->generateId(userEntity))
        {       
          entityData_.userId = newEntityId.get();
          QSqlQuery query(databaseModel()->database());              
          if (query.prepare(querySql))
          {
            query.bindValue(0, entityData_.userId);
            query.bindValue(1, entityData_.userName);
            query.bindValue(2, entityData_.firstName);
            query.bindValue(3, entityData_.lastName);
            query.bindValue(4, entityData_.middleName ? entityData_.middleName.get() : QVariant(QVariant::String));
            query.bindValue(5, passwordHash_.get());          
            query.bindValue(6, databaseModel()->userId());
            query.bindValue(7, databaseModel()->userId());
            if (query.exec())
            {
              if (0 < query.numRowsAffected())
              {
                setEntityId(entityData_.userId);
                databaseModel()->notifyInsert(entityData_);
                return true;
              }            
            }
          }
        }
      }
      return false;
    }

    bool UserCardPage::removeFromDatabase()
    {
      static const char* querySql = "delete from \"USER\" where \"USER_ID\" = ?";

      if (QMessageBox::Yes == QMessageBox::question(this, tr("Users"), 
        tr("Delete user?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
      {
        if (entityId())
        {
          QSqlQuery query(databaseModel()->database());              
          if (query.prepare(querySql))
          {
            query.bindValue(0, entityData_.userId);
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

    bool UserCardPage::checkAccess(bool self)
    {
      static const char* errorMessageTitle = QT_TR_NOOP("Unexpected error");
      static const char* errorMessage = QT_TR_NOOP("Error during work with database.");
      static const char* noRightsMessageTitle = QT_TR_NOOP("Access denied");
      static const char* noRightsMessage = QT_TR_NOOP("You have not enough rights to modify selected user.");

      static const char* querySql = "select u.\"CREATE_USER_ID\" from \"USER\" u where u.\"USER_ID\" = ?";

      bool result = false;
      bool error = false;

      if (OptionalQInt64 optionalUserId = entityId())
      {
        qint64 userId = optionalUserId.get();
        qint64 currentUserId = databaseModel()->userId();  
        if (userId == currentUserId)
        {
          result = self;
        }
        else
        {        
          if (entityData_.createUserId)
          {
            userId = entityData_.createUserId.get();
            if (userId == currentUserId)
            {
              result = true;
            }          
            else
            {            
              QSqlQuery query(databaseModel()->database());              
              if (!query.prepare(querySql))
              {
                error = true;                
              }
              else
              {            
                bool done = false;
                while (!done) 
                {            
                  query.bindValue(0, userId);
                  if (!query.exec())
                  {                    
                    error = true;
                    done = true;
                  }
                  else
                  {
                    if (!query.next())
                    {
                      done = true;
                    }
                    else
                    {
                      if (query.isNull(0))
                      {
                        done = true;
                      }
                      else
                      {
                        userId = query.value(0).toLongLong();
                        if (userId == currentUserId)
                        {
                          result = true;
                          done = true;
                        }
                      }
                    }
                  }
                  query.finish();
                }
              }
            }
          }
        }
      }
      if (error)
      {
        QMessageBox::critical(this, tr(errorMessageTitle), tr(errorMessage));
      }
      else if (!result)
      {
        QMessageBox::warning(this, tr(noRightsMessageTitle), tr(noRightsMessage));
      }
      return result;
    }

    void UserCardPage::on_lineEdit_textEdited(const QString&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void UserCardPage::on_changePasswordCheck_stateChanged(int /*state*/)
    {
      updateWidgets();
    }

    void UserCardPage::on_database_userUpdated(const model::User& user)
    {
      bool updated = false;
      if (entityId())
      {
        if (entityData_.userId != user.userId)
        {
          if (entityData_.createUserId)
          {
            if (entityData_.createUserId.get() == user.userId)
            {
              createUserName_ = user.userName;
              ui_.createUserEdit->setText(user.userName);
              updated = true;
            }
          }
          if (entityData_.updateUserId)
          {
            if (entityData_.updateUserId.get() == user.userId)
            {
              updateUserName_ = user.userName;
              ui_.updateUserEdit->setText(user.userName);
              updated = true;
            }
          }
        }
        if (updated)
        {          
          updateWidgets();
        }
      }
    }    

    void UserCardPage::showRequiredMarkers(bool visible)
    {
      WidgetUtility::show(requiredMarkers_, visible);
    }

    void UserCardPage::on_createUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {
        if (entityData_.createUserId)
        {
          showUserCardTaskWindow(entityData_.createUserId.get());
        }        
      }      
    }

    void UserCardPage::on_updateUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {
        if (entityData_.updateUserId)
        {
          showUserCardTaskWindow(entityData_.updateUserId.get());
        }        
      }
    }

  } // namespace chrono
} // namespace ma
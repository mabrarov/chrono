/*
TRANSLATOR ma::chrono::CounterpartCardPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
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
#include <ma/chrono/listselectdialog.h>
#include <ma/chrono/countrylistpage.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/country.h>
#include <ma/chrono/counterpartcardpage.h>

namespace ma
{
  namespace chrono
  {    
    CounterpartCardPage::CounterpartCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent)
      : CardPage(resourceManager, taskWindowManager, databaseModel, helpAction, counterpartEntity, mode, entityId, parent)
      , dataAwareWidgets_()
      , contentsChanged_(false)
    {
      ui_.setupUi(this);
      ui_.tabWidget->setCurrentIndex(0);      
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      createAction()->setText(tr("&New counterpart"));
      dataAwareWidgets_ << ui_.fullNameEdit << ui_.fullLatinNameEdit 
                        << ui_.shortNameEdit << ui_.shortLatinNameEdit;
      requiredMarkers_  << ui_.fullNameAsterikLabel << ui_.fullLatinNameAsterikLabel
                        << ui_.shortNameAsterikLabel << ui_.shortLatinAsterikLabel
                        << ui_.countryAsterikLabel;
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

    CounterpartCardPage::~CounterpartCardPage()
    {
    }

    void CounterpartCardPage::refresh()
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

    void CounterpartCardPage::save()
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

    void CounterpartCardPage::cancel()
    {
      if (editMode == mode())
      {      
        populateWidgets();
      }      
      CardPage::cancel();
      updateWidgets();
    }

    void CounterpartCardPage::edit()
    {
      CardPage::edit();
      updateWidgets();
    }

    void CounterpartCardPage::remove()
    {
      if (createMode != mode())
      {
        if (removeFromDatabase())
        {
          CardPage::remove();
        }
      }            
    }

    std::auto_ptr<CardPage> CounterpartCardPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new CounterpartCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }

    void CounterpartCardPage::connectDataAwareWidgets()
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

    void CounterpartCardPage::updateWidgets()
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
          updateWindowTitle(tr("Counterparts - %1").arg(entityData_.shortName));
        }
        showRequiredMarkers(false);
        break;

      case editMode:
        if (entityId())
        {
          if (contentsChanged_)
          {
            updateWindowTitle(tr("Counterparts - %1* - edit").arg(entityData_.shortName));
          }
          else
          {
            updateWindowTitle(tr("Counterparts - %1 - edit").arg(entityData_.shortName));
          }        
        }
        showRequiredMarkers(true);
        break;

      case createMode:
        updateWindowTitle(tr("Counterparts - New counterpart creation*"));
        showRequiredMarkers(true);
        break;
      }      
    }

    bool CounterpartCardPage::populateFromWidgets()
    {      
      static const char* fullNameTitle = QT_TR_NOOP("Full name");
      static const char* fullLatinNameTitle = QT_TR_NOOP("Full latin name");
      static const char* shortNameTitle = QT_TR_NOOP("Short name");
      static const char* shortLatinNameTitle = QT_TR_NOOP("Short latin name");
      static const char* countryTitle = QT_TR_NOOP("Country");
      
      static const char* messageTitle = QT_TR_NOOP("Invalid data input");
      static const char* notEmpty = QT_TR_NOOP("Field \"%1\" can\'t be empty.");
      static const char* tooLongText = QT_TR_NOOP("Field \"%1\" can\'t exceed %2 characters.");      

      int contentsTabIndex = ui_.tabWidget->indexOf(ui_.contentsTab);
      bool validated;
      // Get full name
      QString fullName = ui_.fullNameEdit->text().trimmed();
      if (fullName.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(fullNameTitle)));
      } 
      else if (fullName.length() > model::Counterpart::fullNameMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(fullNameTitle)).arg(model::Counterpart::fullNameMaxLen));
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
      // Get full latin name
      QString fullLatinName = ui_.fullLatinNameEdit->text().trimmed();
      if (fullLatinName.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(fullLatinNameTitle)));
      } 
      else if (fullLatinName.length() > model::Counterpart::fullLatinNameMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(fullLatinNameTitle)).arg(model::Counterpart::fullLatinNameMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.fullLatinNameEdit->setFocus(Qt::TabFocusReason);
        ui_.fullLatinNameEdit->selectAll();
        return false;
      }
      // Get short name
      QString shortName = ui_.shortNameEdit->text().trimmed();
      if (shortName.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(shortNameTitle)));
      } 
      else if (shortName.length() > model::Counterpart::shortNameMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(shortNameTitle)).arg(model::Counterpart::shortNameMaxLen));
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
      // Get short latin name
      QString shortLatinName = ui_.shortLatinNameEdit->text().trimmed();
      if (shortLatinName.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(shortLatinNameTitle)));
      } 
      else if (shortLatinName.length() > model::Counterpart::shortLatinNameMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(shortLatinNameTitle)).arg(model::Counterpart::shortLatinNameMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.shortLatinNameEdit->setFocus(Qt::TabFocusReason);
        ui_.shortLatinNameEdit->selectAll();
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
      entityData_.fullName       = fullName;
      entityData_.fullLatinName  = fullLatinName;
      entityData_.shortName      = shortName;
      entityData_.shortLatinName = shortLatinName;
      entityData_.countryId      = selectedCountryId_.get(); 
      return true;
    }

    bool CounterpartCardPage::populateFromDatabase()
    {      
      static const char* querySql = "select " \
        "t.\"FULL_NAME\", t.\"FULL_LATIN_NAME\", "
        "t.\"SHORT_NAME\", t.\"SHORT_LATIN_NAME\", " \
        "t.\"COUNTRY_ID\", c.\"FULL_NAME\", " \
        "t.\"CREATE_USER_ID\", cu.\"LOGIN\", " \
        "t.\"UPDATE_USER_ID\", uu.\"LOGIN\", " \
        "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
        "from \"COUNTERPART\" t " \
        "join \"COUNTRY\" c on t.\"COUNTRY_ID\" = c.\"COUNTRY_ID\" " \
        "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
        "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
        "where t.\"COUNTERPART_ID\" = ?";

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
              entityData_.counterpartId  = entityId().get();
              entityData_.fullName       = query.value(0).toString();
              entityData_.fullLatinName  = query.value(1).toString();
              entityData_.shortName      = query.value(2).toString();
              entityData_.shortLatinName = query.value(3).toString();
              entityData_.countryId = query.value(4).toLongLong();
              countryFullName_      = query.value(5).toString();              
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

    void CounterpartCardPage::populateWidgets()
    {
      if (entityId())
      {
        // Contents page
        ui_.fullNameEdit->setText(entityData_.fullName);
        ui_.fullLatinNameEdit->setText(entityData_.fullLatinName);
        ui_.shortNameEdit->setText(entityData_.shortName);
        ui_.shortLatinNameEdit->setText(entityData_.shortLatinName);        
        ui_.countryEdit->setText(countryFullName_);
        selectedCountryId_ = entityData_.countryId;
        // General data page
        ui_.createTimeEdit->setText(entityData_.createTime.toString(Qt::DefaultLocaleShortDate));
        ui_.updateTimeEdit->setText(entityData_.updateTime.toString(Qt::DefaultLocaleShortDate));
        ui_.createUserEdit->setText(createUserName_);
        ui_.updateUserEdit->setText(updateUserName_);
        populateExportData();
        contentsChanged_ = false;
      }      
    } 

    void CounterpartCardPage::populateExportData()
    {      
      static const QString fullNameCaption = tr("Full name");
      static const QString fullLatinNameCaption = tr("Full latin name");
      static const QString shortNameCaption = tr("Short name");
      static const QString shortLatinNameCaption = tr("Short latin name");
      static const QString countryCaption = tr("Country");      
      static const QString createUserCaption = tr("User, creator");
      static const QString createTimeCaption = tr("Time, created");
      static const QString updateUserCaption = tr("User, last update");
      static const QString updateTimeCaption = tr("Time, last update");
        
      ExportDataList exportDataList;
      exportDataList << std::make_pair(fullNameCaption, QVariant(entityData_.fullName));
      exportDataList << std::make_pair(fullLatinNameCaption, QVariant(entityData_.fullLatinName));
      exportDataList << std::make_pair(shortNameCaption, QVariant(entityData_.shortName));
      exportDataList << std::make_pair(shortLatinNameCaption, QVariant(entityData_.shortLatinName));
      exportDataList << std::make_pair(countryCaption, QVariant(countryFullName_));
      exportDataList << std::make_pair(createUserCaption, QVariant(createUserName_));
      exportDataList << std::make_pair(createTimeCaption, QVariant(entityData_.createTime));
      exportDataList << std::make_pair(updateUserCaption, QVariant(updateUserName_));
      exportDataList << std::make_pair(updateTimeCaption, QVariant(entityData_.updateTime));      
            
      setExportDataList(exportDataList);
    }

    bool CounterpartCardPage::updateDatabase()
    {      
      static const char* querySql = "update \"COUNTERPART\" set " \
        "\"FULL_NAME\" = ?, \"FULL_LATIN_NAME\" = ?, " \
        "\"SHORT_NAME\" = ?, \"SHORT_LATIN_NAME\" = ?, " \
        "\"COUNTRY_ID\" = ?, \"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\' " \
        "where \"COUNTERPART_ID\" = ?";

      if (entityId())
      {
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.fullName);
          query.bindValue(1, entityData_.fullLatinName);
          query.bindValue(2, entityData_.shortName);
          query.bindValue(3, entityData_.shortLatinName);
          query.bindValue(4, entityData_.countryId);
          query.bindValue(5, databaseModel()->userId());
          query.bindValue(6, entityData_.counterpartId);
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

    bool CounterpartCardPage::insertIntoDatabase()
    {      
      static const char* querySql = "insert into \"COUNTERPART\"(" \
        "\"COUNTERPART_ID\", \"FULL_NAME\", \"FULL_LATIN_NAME\", " \
        "\"SHORT_NAME\", \"SHORT_LATIN_NAME\", " \
        "\"COUNTRY_ID\", \"CREATE_USER_ID\", \"UPDATE_USER_ID\", \"CREATE_TIME\", \"UPDATE_TIME\") " \
        "values (?, ?, ?, ?, ?, ?, ?, ?, \'NOW\', \'NOW\')";
      
      if (OptionalQInt64 newEntityId = databaseModel()->generateId(currencyEntity))
      {
        entityData_.counterpartId = newEntityId.get();
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.counterpartId);
          query.bindValue(1, entityData_.fullName);
          query.bindValue(2, entityData_.fullLatinName);
          query.bindValue(3, entityData_.shortName);
          query.bindValue(4, entityData_.shortLatinName);
          query.bindValue(5, entityData_.countryId);                    
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

    bool CounterpartCardPage::removeFromDatabase()
    {      
      static const char* querySql = "delete from \"COUNTERPART\" where \"COUNTERPART_ID\" = ?";

      if (QMessageBox::Yes == QMessageBox::question(this, tr("Counterparts"), 
        tr("Delete counterpart?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
      {
        if (entityId())
        {
          QSqlQuery query(databaseModel()->database());              
          if (query.prepare(querySql))
          {
            query.bindValue(0, entityData_.counterpartId);
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

    void CounterpartCardPage::on_lineEdit_textEdited(const QString&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void CounterpartCardPage::on_database_userUpdated(const model::User& user)
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

    void CounterpartCardPage::on_database_countryUpdated(const model::Country& country)
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

    void CounterpartCardPage::on_database_countryRemoved(const ma::chrono::model::Country& country)
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

    void CounterpartCardPage::showRequiredMarkers(bool visible)
    {
      WidgetUtility::show(requiredMarkers_, visible);
    }

    void CounterpartCardPage::on_createUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.createUserId);
      }        
    }

    void CounterpartCardPage::on_updateUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.updateUserId);
      }        
    }

    void CounterpartCardPage::on_countryBtn_clicked(bool /*checked*/)
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
} //namespace ma
/*
TRANSLATOR ma::chrono::TtermCardPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
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
#include <ma/chrono/model/user.h>
#include <ma/chrono/ttermcardpage.h>

namespace ma
{
  namespace chrono
  {    
    TtermCardPage::TtermCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent)
      : CardPage(resourceManager, taskWindowManager, databaseModel, helpAction, ttermEntity, mode, entityId, parent)
      , dataAwareWidgets_()
      , contentsChanged_(false)
    {
      ui_.setupUi(this);
      ui_.tabWidget->setCurrentIndex(0);
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      createAction()->setText(tr("&New transaction terms"));
      dataAwareWidgets_ << ui_.nameEdit << ui_.remarksText;                        
      requiredMarkers_  << ui_.nameAsterikLabel;
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

    TtermCardPage::~TtermCardPage()
    {
    }
     
    void TtermCardPage::refresh()
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

    void TtermCardPage::save()
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

    void TtermCardPage::cancel()
    {
      if (editMode == mode())
      {      
        populateWidgets();
      }      
      CardPage::cancel();
      updateWidgets();
    }

    void TtermCardPage::edit()
    {
      CardPage::edit();
      updateWidgets();
    }

    void TtermCardPage::remove()
    {
      if (createMode != mode())
      {
        if (removeFromDatabase())
        {
          CardPage::remove();
        }
      }            
    }

    std::auto_ptr<CardPage> TtermCardPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new TtermCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }

    void TtermCardPage::connectDataAwareWidgets()
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
      }
    }

    void TtermCardPage::updateWidgets()
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
        else if (QTextEdit* textEdit = qobject_cast<QTextEdit*>(widget))
        {
          textEdit->setReadOnly(readOnly);
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
          updateWindowTitle(tr("Transaction terms - %1").arg(entityData_.name));
        } 
        showRequiredMarkers(false);
        break;

      case editMode:
        if (entityId())
        {
          if (contentsChanged_)
          {
            updateWindowTitle(tr("Transaction terms - %1* - edit").arg(entityData_.name));
          }
          else
          {
            updateWindowTitle(tr("Transaction terms - %1 - edit").arg(entityData_.name));
          }        
        }
        showRequiredMarkers(true);
        break;

      case createMode:
        updateWindowTitle(tr("Transaction terms - New transaction terms creation*"));
        showRequiredMarkers(true);
        break;
      }      
    }

    bool TtermCardPage::populateFromWidgets()
    {      
      static const char* nameTitle = QT_TR_NOOP("Name");
      static const char* remarkTitle = QT_TR_NOOP("Remarks");      

      static const char* messageTitle = QT_TR_NOOP("Invalid data input");
      static const char* notEmpty = QT_TR_NOOP("Field \"%1\" can\'t be empty.");
      static const char* tooLongText = QT_TR_NOOP("Field \"%1\" can\'t exceed %2 characters.");      

      int contentsTabIndex = ui_.tabWidget->indexOf(ui_.contentsTab);
      bool validated;
      // Get name
      QString name = ui_.nameEdit->text().trimmed();
      if (name.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(nameTitle)));
      } 
      else if (name.length() > model::Tterm::nameMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(nameTitle)).arg(model::Tterm::nameMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.nameEdit->setFocus(Qt::TabFocusReason);
        ui_.nameEdit->selectAll();
        return false;
      }
      // Get remarks
      QString remark = ui_.remarksText->toPlainText().trimmed();      
      if (remark.length() > model::Tterm::remarkMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(remarkTitle)).arg(model::Tterm::remarkMaxLen));                
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.remarksText->setFocus(Qt::TabFocusReason);
        ui_.remarksText->selectAll();
        return false;
      }      
      entityData_.name   = name;
      entityData_.remark = remark.isEmpty() ? OptionalQString() : remark;
      return true;
    }

    bool TtermCardPage::populateFromDatabase()
    {
      static const char* querySql = "select " \
        "t.\"NAME\", t.\"REMARK\", " \
        "t.\"CREATE_USER_ID\", cu.\"LOGIN\", " \
        "t.\"UPDATE_USER_ID\", uu.\"LOGIN\", " \
        "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
        "from \"TTERM\" t " \
        "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
        "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
        "where t.\"TTERM_ID\" = ?";

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
              entityData_.ttermId = entityId().get();
              entityData_.name  = query.value(0).toString();
              if (query.isNull(1))
              {
                entityData_.remark.reset();
              }
              else
              {
                entityData_.remark = query.value(1).toString();              
              }              
              entityData_.createUserId = query.value(2).toLongLong();
              createUserName_          = query.value(3).toString();
              entityData_.updateUserId = query.value(4).toLongLong();
              updateUserName_          = query.value(5).toString();              
              entityData_.createTime   = databaseModel()->convertFromServer(query.value(6).toDateTime());
              entityData_.updateTime   = databaseModel()->convertFromServer(query.value(7).toDateTime());
              return true;
            }
          }
        }
      }
      return false;
    }

    void TtermCardPage::populateWidgets()
    {
      if (entityId())
      {      
        // Contents page
        ui_.nameEdit->setText(entityData_.name);
        ui_.remarksText->setPlainText(entityData_.remark ? entityData_.remark.get() : QString());
        // General data page
        ui_.createTimeEdit->setText(entityData_.createTime.toString(Qt::DefaultLocaleShortDate));
        ui_.updateTimeEdit->setText(entityData_.updateTime.toString(Qt::DefaultLocaleShortDate));
        ui_.createUserEdit->setText(createUserName_);
        ui_.updateUserEdit->setText(updateUserName_);        
        populateExportData();
        contentsChanged_ = false;
      }
    } 

    void TtermCardPage::populateExportData()
    {
      static QString nameCaption = tr("Name");
      static QString remarksCaption = tr("Remarks");      
      static QString createUserCaption = tr("User, creator");
      static QString createTimeCaption = tr("Time, created");
      static QString updateUserCaption = tr("User, last update");
      static QString updateTimeCaption = tr("Time, last update");                    
        
      ExportDataList exportDataList;
      exportDataList << std::make_pair(nameCaption, QVariant(entityData_.name));
      exportDataList << std::make_pair(remarksCaption, entityData_.remark ? QVariant(entityData_.remark.get()) : QVariant(QVariant::String));
      exportDataList << std::make_pair(createUserCaption, QVariant(createUserName_));
      exportDataList << std::make_pair(createTimeCaption, QVariant(entityData_.createTime));
      exportDataList << std::make_pair(updateUserCaption, QVariant(updateUserName_));
      exportDataList << std::make_pair(updateTimeCaption, QVariant(entityData_.updateTime));      

      setExportDataList(exportDataList);
    }

    bool TtermCardPage::updateDatabase()
    {
      static const char* querySql = "update \"TTERM\" set " \
        "\"NAME\" = ?, \"REMARK\" = ?, " \
        "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\' " \
        "where \"TTERM_ID\" = ?";

      if (entityId())
      {
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.name);
          query.bindValue(1, entityData_.remark ? entityData_.remark.get() : QVariant(QVariant::String));
          query.bindValue(2, databaseModel()->userId());
          query.bindValue(3, entityData_.ttermId);
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

    bool TtermCardPage::insertIntoDatabase()
    {
      static const char* querySql = "insert into \"TTERM\"(" \
        "\"TTERM_ID\", \"NAME\", \"REMARK\", " \
        "\"CREATE_USER_ID\", \"UPDATE_USER_ID\", \"CREATE_TIME\", \"UPDATE_TIME\") " \
        "values (?, ?, ?, ?, ?, \'NOW\', \'NOW\')";
      
      if (OptionalQInt64 newEntityId = databaseModel()->generateId(ttermEntity))
      {
        entityData_.ttermId = newEntityId.get();
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.ttermId);
          query.bindValue(1, entityData_.name);
          query.bindValue(2, entityData_.remark ? entityData_.remark.get() : QVariant(QVariant::String));          
          query.bindValue(3, databaseModel()->userId());
          query.bindValue(4, databaseModel()->userId());
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

    bool TtermCardPage::removeFromDatabase()
    {
      static const char* querySql = "delete from \"TTERM\" where \"TTERM_ID\" = ?";

      if (QMessageBox::Yes == QMessageBox::question(this, tr("Transaction terms"), 
        tr("Delete transaction terms?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
      {
        if (entityId())
        {
          QSqlQuery query(databaseModel()->database());              
          if (query.prepare(querySql))
          {
            query.bindValue(0, entityData_.ttermId);
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

    void TtermCardPage::on_lineEdit_textEdited(const QString&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void TtermCardPage::on_textEdit_textChanged()
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void TtermCardPage::on_database_userUpdated(const model::User& user)
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

    void TtermCardPage::showRequiredMarkers(bool visible)
    {
      WidgetUtility::show(requiredMarkers_, visible);
    }

    void TtermCardPage::on_createUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.createUserId);
      }        
    }

    void TtermCardPage::on_updateUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.updateUserId);
      }        
    }

  } // namespace chrono
} //namespace ma
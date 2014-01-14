/*
TRANSLATOR ma::chrono::AutoenterpriseCardPage
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
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
#include <ma/chrono/model/user.h>
#include <ma/chrono/autoenterprisecardpage.h>

namespace ma {
namespace chrono {

AutoenterpriseCardPage::AutoenterpriseCardPage(
    const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerWeakPtr& taskWindowManager,
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, Mode mode, 
    const OptionalQInt64& entityId, QWidget* parent)
  : CardPage(resourceManager, taskWindowManager, databaseModel, helpAction, 
        autoenterpriseEntity, mode, entityId, parent)
  , dataAwareWidgets_()
  , contentsChanged_(false)
{
  ui_.setupUi(this);
  ui_.tabWidget->setCurrentIndex(0);
  setWindowIcon(resourceManager->getIcon(chronoIconName));
  createAction()->setText(tr("&New autoenterprise"));

  dataAwareWidgets_ << ui_.fullNameEdit 
                    << ui_.shortNameEdit 
                    << ui_.remarksText;
  connectDataAwareWidgets(dataAwareWidgets_);

  requiredMarkers_  << ui_.fullNameAsterikLabel 
                    << ui_.shortNameAsterikLabel;  

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

  QObject::connect(databaseModel.get(), 
      SIGNAL(userUpdated(const ma::chrono::model::User&)), 
      SLOT(on_database_userUpdated(const ma::chrono::model::User&)));

  updateWidgets();
}


AutoenterpriseCardPage::~AutoenterpriseCardPage()
{
}

void AutoenterpriseCardPage::refresh()
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

void AutoenterpriseCardPage::save()
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

void AutoenterpriseCardPage::cancel()
{
  if (editMode == mode())
  {      
    populateWidgets();
  }      
  CardPage::cancel();
  updateWidgets();
}

void AutoenterpriseCardPage::edit()
{      
  CardPage::edit();
  updateWidgets();
}

void AutoenterpriseCardPage::remove()
{
  if (createMode != mode())
  {
    if (removeFromDatabase())
    {
      CardPage::remove();
    }
  }            
}

std::auto_ptr<CardPage> AutoenterpriseCardPage::createCardPage(
    CardPage::Mode mode, const OptionalQInt64& entityId) const
{
  std::auto_ptr<CardPage> cardPage(new AutoenterpriseCardPage(
      resourceManager(), taskWindowManager(), databaseModel(), helpAction(),
      mode, entityId));
  return cardPage;
}           

void AutoenterpriseCardPage::connectDataAwareWidgets(
    const QWidgetList& widgets)
{  
  typedef QWidgetList::const_iterator const_iterator;
      
  for (const_iterator i = widgets.begin(), end = widgets.end(); i != end; ++i)
  {
    QWidget* widget = *i;
    if (QLineEdit* edit = qobject_cast<QLineEdit*>(widget))
    {
      QObject::connect(edit, SIGNAL(textEdited(const QString&)), 
          SLOT(on_lineEdit_textEdited(const QString&)));
    }
    else if (QTextEdit* edit = qobject_cast<QTextEdit*>(widget))
    {
      QObject::connect(edit, SIGNAL(textChanged()), 
          SLOT(on_textEdit_textChanged()));
    }
  }
}

void AutoenterpriseCardPage::updateWidgets()
{
  const Mode currentMode = mode();

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
      updateWindowTitle(tr("Autoenterprises - %1").arg(entityData_.shortName));
    }        
    showRequiredMarkers(false);
    break;

  case editMode:
    if (entityId())
    {
      if (contentsChanged_)
      {
        updateWindowTitle(tr("Autoenterprises - %1* - edit")
            .arg(entityData_.shortName));
      }
      else
      {
        updateWindowTitle(tr("Autoenterprises - %1 - edit")
            .arg(entityData_.shortName));
      }        
    }
    showRequiredMarkers(true);
    break;

  case createMode:
    updateWindowTitle(tr("Autoenterprises - New autoenterprise*"));
    showRequiredMarkers(true);
    break;
  }      
}

bool AutoenterpriseCardPage::populateFromWidgets()
{      
  static const char* fullNameTitle = QT_TR_NOOP("Full name");
  static const char* shortNameTitle = QT_TR_NOOP("Short name");
  static const char* remarkTitle = QT_TR_NOOP("Remarks");      

  static const char* messageTitle = 
      QT_TR_NOOP("Invalid data input");
  static const char* notEmpty = 
      QT_TR_NOOP("Field \"%1\" can\'t be empty.");
  static const char* tooLongText = 
      QT_TR_NOOP("Field \"%1\" can\'t exceed %2 characters.");      

  int contentsTabIndex = ui_.tabWidget->indexOf(ui_.contentsTab);
  bool validated;
  // Get full name
  QString fullName = ui_.fullNameEdit->text().trimmed();
  if (fullName.isEmpty())
  {
    validated = false;
    QMessageBox::warning(this, tr(messageTitle), 
        tr(notEmpty).arg(tr(fullNameTitle)));
  } 
  else if (fullName.length() > model::Autoenterprise::fullNameMaxLen)
  {
    validated = false;
    QMessageBox::warning(this, tr(messageTitle), 
        tr(tooLongText).arg(tr(fullNameTitle))
            .arg(model::Autoenterprise::fullNameMaxLen));
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
    QMessageBox::warning(this, tr(messageTitle), 
        tr(notEmpty).arg(tr(shortNameTitle)));
  } 
  else if (shortName.length() > model::Autoenterprise::shortNameMaxLen)
  {
    validated = false;
    QMessageBox::warning(this, tr(messageTitle), 
        tr(tooLongText).arg(tr(shortNameTitle))
            .arg(model::Autoenterprise::shortNameMaxLen));
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
  // Get remarks
  QString remark = ui_.remarksText->toPlainText().trimmed();      
  if (remark.length() > model::Autoenterprise::remarkMaxLen)
  {
    validated = false;
    QMessageBox::warning(this, tr(messageTitle), 
        tr(tooLongText).arg(tr(remarkTitle))
            .arg(model::Autoenterprise::remarkMaxLen));                
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
  entityData_.fullName  = fullName;
  entityData_.shortName = shortName;
  entityData_.remark    = buildOptionalQString(remark);
  return true;
}

bool AutoenterpriseCardPage::populateFromDatabase()
{
  static const char* querySql = "select " \
      "t.\"FULL_NAME\", t.\"SHORT_NAME\", t.\"REMARK\", " \
      "t.\"CREATE_USER_ID\", cu.\"LOGIN\", " \
      "t.\"UPDATE_USER_ID\", uu.\"LOGIN\", " \
      "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
      "from \"AUTOENTERPRISE\" t " \
      "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
      "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
      "where t.\"AUTOENTERPRISE_ID\" = ?";

  if (!entityId())
  {
    return false;
  }

  QSqlQuery query(databaseModel()->database());
  if (!query.prepare(querySql))
  {
    return false;
  }

  query.bindValue(0, entityId().get());
  if (!query.exec())
  {
    return false;
  }

  if (!query.next())
  {
    return false;
  }
           
  entityData_.autoenterpriseId = entityId().get();
  entityData_.fullName     = query.value(0).toString();
  entityData_.shortName    = query.value(1).toString();
  entityData_.remark       = SqlUtility::getOptionalQString(query, 2);
  entityData_.createUserId = query.value(3).toLongLong();
  createUserName_          = query.value(4).toString();
  entityData_.updateUserId = query.value(5).toLongLong();
  updateUserName_          = query.value(6).toString();              
  entityData_.createTime   = databaseModel()->convertFromServer(
      query.value(7).toDateTime());
  entityData_.updateTime   = databaseModel()->convertFromServer(
      query.value(8).toDateTime());
  return true;
}

void AutoenterpriseCardPage::populateWidgets()
{
  if (entityId())
  {      
    // Contents page
    ui_.fullNameEdit->setText(entityData_.fullName);
    ui_.shortNameEdit->setText(entityData_.shortName);
    ui_.remarksText->setPlainText(buildQString(entityData_.remark));
    // General data page
    ui_.createTimeEdit->setText(
        entityData_.createTime.toString(Qt::DefaultLocaleShortDate));
    ui_.updateTimeEdit->setText(
        entityData_.updateTime.toString(Qt::DefaultLocaleShortDate));
    ui_.createUserEdit->setText(createUserName_);
    ui_.updateUserEdit->setText(updateUserName_);
    populateExportData();
    contentsChanged_ = false;
  }
}

void AutoenterpriseCardPage::populateExportData()
{
  static const QString fullNameCaption = tr("Full name");
  static const QString shortNameCaption = tr("Short name");
  static const QString remarksCaption = tr("Remarks");      
  static const QString createUserCaption = tr("User, creator");
  static const QString createTimeCaption = tr("Time, created");
  static const QString updateUserCaption = tr("User, last update");
  static const QString updateTimeCaption = tr("Time, last update");                    
        
  ExportDataList exportDataList;
  exportDataList << std::make_pair(
      fullNameCaption, QVariant(entityData_.fullName));
  exportDataList << std::make_pair(
      shortNameCaption, QVariant(entityData_.shortName));
  exportDataList << std::make_pair(
      remarksCaption, buildQVariant(entityData_.remark));
  exportDataList << std::make_pair(
      createUserCaption, QVariant(createUserName_));
  exportDataList << std::make_pair(
      createTimeCaption, QVariant(entityData_.createTime));
  exportDataList << std::make_pair(
      updateUserCaption, QVariant(updateUserName_));
  exportDataList << std::make_pair(
      updateTimeCaption, QVariant(entityData_.updateTime));

  setExportDataList(exportDataList);
}
   
bool AutoenterpriseCardPage::updateDatabase()
{
  static const char* querySql = "update \"AUTOENTERPRISE\" set " \
      "\"FULL_NAME\" = ?, \"SHORT_NAME\" = ?, \"REMARK\" = ?, " \
      "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\' " \
      "where \"AUTOENTERPRISE_ID\" = ?";

  if (!entityId())
  {
    return false;
  }

  QSqlQuery query(databaseModel()->database());              
  if (!query.prepare(querySql))
  {
    return false;
  }
    
  query.bindValue(0, entityData_.fullName);
  query.bindValue(1, entityData_.shortName);
  query.bindValue(2, buildQVariant(entityData_.remark));
  query.bindValue(3, databaseModel()->userId());
  query.bindValue(4, entityData_.autoenterpriseId);
  if (!query.exec())
  {
    return false;
  }
      
  if (query.numRowsAffected() > 0)
  {
    databaseModel()->notifyUpdate(entityData_);
    return true;
  }  
  return false;
}

bool AutoenterpriseCardPage::insertIntoDatabase()
{
  static const char* querySql = "insert into \"AUTOENTERPRISE\"(" \
      "\"AUTOENTERPRISE_ID\", \"FULL_NAME\", \"SHORT_NAME\", \"REMARK\", " \
      "\"CREATE_USER_ID\", \"UPDATE_USER_ID\", " \
      "\"CREATE_TIME\", \"UPDATE_TIME\") " \
      "values (?, ?, ?, ?, ?, ?, \'NOW\', \'NOW\')";
      
  if (OptionalQInt64 newEntityId = 
      databaseModel()->generateId(autoenterpriseEntity))
  {
    entityData_.autoenterpriseId = newEntityId.get();
    QSqlQuery query(databaseModel()->database());              
    if (!query.prepare(querySql))
    {
      return false;
    }

    query.bindValue(0, entityData_.autoenterpriseId);
    query.bindValue(1, entityData_.fullName);
    query.bindValue(2, entityData_.shortName);
    query.bindValue(3, buildQVariant(entityData_.remark));
    query.bindValue(4, databaseModel()->userId());
    query.bindValue(5, databaseModel()->userId());
    if (!query.exec())
    {
      return false;
    }

    if (query.numRowsAffected() > 0)
    {
      setEntityId(newEntityId.get());
      databaseModel()->notifyInsert(entityData_);
      return true;
    }    
  }
  return false;
}

bool AutoenterpriseCardPage::removeFromDatabase()
{
  static const char* querySql = 
      "delete from \"AUTOENTERPRISE\" where \"AUTOENTERPRISE_ID\" = ?";

  if (QMessageBox::Yes == QMessageBox::question(this, tr("Autoenterprises"), 
      tr("Delete autoenterprise?"), QMessageBox::Yes | QMessageBox::No, 
      QMessageBox::No))
  {
    if (!entityId())
    {
      return false;
    }

    QSqlQuery query(databaseModel()->database());              
    if (!query.prepare(querySql))
    {
      return false;
    }

    query.bindValue(0, entityData_.autoenterpriseId);
    if (!query.exec())
    {
      return false;
    }
      
    setEntityId(OptionalQInt64());

    if (query.numRowsAffected() > 0)
    {
      databaseModel()->notifyRemove(entityData_);
      return true;
    }            
  }
  return false;
}

void AutoenterpriseCardPage::on_lineEdit_textEdited(const QString&)
{
  contentsChanged_ = true;
  updateWidgets();
}

void AutoenterpriseCardPage::on_textEdit_textChanged()
{
  contentsChanged_ = true;
  updateWidgets();
}        

void AutoenterpriseCardPage::showRequiredMarkers(bool visible)
{
  WidgetUtility::show(requiredMarkers_, visible);
}

void AutoenterpriseCardPage::on_createUserBtn_clicked(bool /*checked*/)
{
  if (entityId())
  {      
    showUserCardTaskWindow(entityData_.createUserId);
  }        
}

void AutoenterpriseCardPage::on_updateUserBtn_clicked(bool /*checked*/)
{
  if (entityId())
  {      
    showUserCardTaskWindow(entityData_.updateUserId);
  }        
}

void AutoenterpriseCardPage::on_database_userUpdated(const model::User& user)
{
  bool updated = false;
  if (entityId())
  {        
    if (user.userId == entityData_.createUserId)
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

} // namespace chrono
} // namespace ma

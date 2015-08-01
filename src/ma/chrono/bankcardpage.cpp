/*
TRANSLATOR ma::chrono::BankCardPage
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
#include <ma/chrono/model/user.h>
#include <ma/chrono/bankcardpage.h>

namespace ma {
namespace chrono {

BankCardPage::BankCardPage(const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerWeakPtr& taskWindowManager,
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, Mode mode, 
    const OptionalQInt64& entityId, QWidget* parent)
  : CardPage(resourceManager, taskWindowManager, databaseModel, helpAction, 
        bankEntity, mode, entityId, parent)
  , dataAwareWidgets_()
  , contentsChanged_(false)
{
  ui_.setupUi(this);
  ui_.tabWidget->setCurrentIndex(0);
  setWindowIcon(resourceManager->getIcon(chronoIconName));
  createAction()->setText(tr("&New bank"));

  dataAwareWidgets_ << ui_.bankCodeEdit 
                    << ui_.fullNameEdit 
                    << ui_.shortNameEdit 
                    << ui_.remarksText;
  connectDataAwareWidgets(dataAwareWidgets_);

  requiredMarkers_  << ui_.bankCodeAsterikLabel 
                    << ui_.fullNameAsterikLabel 
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


BankCardPage::~BankCardPage()
{
}

void BankCardPage::refresh()
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

void BankCardPage::save()
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

void BankCardPage::cancel()
{
  if (editMode == mode())
  {      
    populateWidgets();
  }      
  CardPage::cancel();
  updateWidgets();
}

void BankCardPage::edit()
{      
  CardPage::edit();
  updateWidgets();
}

void BankCardPage::remove()
{
  if (createMode != mode())
  {
    if (removeFromDatabase())
    {
      CardPage::remove();
    }
  }            
}

std::auto_ptr<CardPage> BankCardPage::createCardPage(CardPage::Mode mode,
    const OptionalQInt64& entityId) const
{
  std::auto_ptr<CardPage> cardPage(new BankCardPage(resourceManager(), 
      taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
  return cardPage;
}

void BankCardPage::connectDataAwareWidgets(const QWidgetList& widgets)
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

void BankCardPage::updateWidgets()
{  
  const Mode currentMode = mode();

  const bool readOnly = viewMode == currentMode;
  WidgetUtility::setReadOnly(dataAwareWidgets_, readOnly);
    
  const bool peristance = createMode != currentMode && entityId();
  int generalTabIndex = ui_.tabWidget->indexOf(ui_.generalTab);
  ui_.tabWidget->setTabEnabled(generalTabIndex, peristance);

  switch (currentMode)
  {
  case viewMode:
    if (entityId())
    {
      updateWindowTitle(tr("Banks - %1").arg(entityData_.fullName));
    }        
    showRequiredMarkers(false);
    break;

  case editMode:
    if (entityId())
    {
      if (contentsChanged_)
      {
        updateWindowTitle(tr("Banks - %1* - edit").arg(entityData_.fullName));
      }
      else
      {
        updateWindowTitle(tr("Banks - %1 - edit").arg(entityData_.fullName));
      }        
    }
    showRequiredMarkers(true);
    break;

  case createMode:
    updateWindowTitle(tr("Banks - New bank*"));
    showRequiredMarkers(true);
    break;
  }      
}

bool BankCardPage::populateFromWidgets()
{      
  static const char* bankCodeTitle  = QT_TR_NOOP("Bank code");
  static const char* fullNameTitle  = QT_TR_NOOP("Full name");
  static const char* shortNameTitle = QT_TR_NOOP("Short name");
  static const char* remarkTitle    = QT_TR_NOOP("Remarks");      

  static const char* messageTitle = 
      QT_TR_NOOP("Invalid data input");
  static const char* notEmpty = 
      QT_TR_NOOP("Field \"%1\" can\'t be empty.");
  static const char* tooLongText = 
      QT_TR_NOOP("Field \"%1\" can\'t exceed %2 characters.");
  static const char* invalidInteger = 
      QT_TR_NOOP("Field \"%1\" must be an integer value: [%2, %3].");

  int contentsTabIndex = ui_.tabWidget->indexOf(ui_.contentsTab);
  bool validated;
  // Get bank code
  int bankCode = ui_.bankCodeEdit->text().trimmed().toInt(&validated);
  if (validated)
  {
    validated = (bankCode >= 0) && (bankCode <= model::Bank::bankCodeMax);
  }
  if (!validated)
  {
    QMessageBox::warning(this, tr(messageTitle), 
        tr(invalidInteger).arg(tr(bankCodeTitle)).arg(0).arg(
            model::Bank::bankCodeMax));
    ui_.tabWidget->setCurrentIndex(contentsTabIndex);
    ui_.bankCodeEdit->setFocus(Qt::TabFocusReason);
    ui_.bankCodeEdit->selectAll();
    return false;
  }
  // Get full name
  QString fullName = ui_.fullNameEdit->text().trimmed();
  if (fullName.isEmpty())
  {
    validated = false;
    QMessageBox::warning(this, tr(messageTitle), 
        tr(notEmpty).arg(tr(fullNameTitle)));
  } 
  else if (fullName.length() > model::Bank::fullNameMaxLen)
  {
    validated = false;
    QMessageBox::warning(this, tr(messageTitle), 
        tr(tooLongText).arg(tr(fullNameTitle)).arg(
            model::Bank::fullNameMaxLen));
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
  else if (shortName.length() > model::Bank::shortNameMaxLen)
  {
    validated = false;
    QMessageBox::warning(this, tr(messageTitle), 
        tr(tooLongText).arg(tr(shortNameTitle)).arg(
            model::Bank::shortNameMaxLen));
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
  if (remark.length() > model::Bank::remarkMaxLen)
  {
    validated = false;
    QMessageBox::warning(this, tr(messageTitle), 
        tr(tooLongText).arg(tr(remarkTitle)).arg(model::Bank::remarkMaxLen));
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
  entityData_.bankCode  = bankCode;
  entityData_.fullName  = fullName;
  entityData_.shortName = shortName;
  entityData_.remark    = buildOptionalQString(remark);      
  return true;
}

bool BankCardPage::populateFromDatabase()
{
  static const char* querySql = "select " \
      "t.\"BANK_CODE\", t.\"FULL_NAME\", t.\"SHORT_NAME\", t.\"REMARK\", " \
      "t.\"CREATE_USER_ID\", cu.\"LOGIN\", " \
      "t.\"UPDATE_USER_ID\", uu.\"LOGIN\", " \
      "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
      "from \"BANK\" t " \
      "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
      "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
      "where t.\"BANK_ID\" = ?";

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

  entityData_.bankId       = entityId().get();
  entityData_.bankCode     = query.value(0).toInt();
  entityData_.fullName     = query.value(1).toString();
  entityData_.shortName    = query.value(2).toString();
  entityData_.remark       = SqlUtility::getOptionalQString(query, 3);
  entityData_.createUserId = query.value(4).toLongLong();
  createUserName_          = query.value(5).toString();
  entityData_.updateUserId = query.value(6).toLongLong();
  updateUserName_          = query.value(7).toString();              
  entityData_.createTime   = databaseModel()->convertFromServer(
      query.value(8).toDateTime());
  entityData_.updateTime   = databaseModel()->convertFromServer(
      query.value(9).toDateTime());
  return true;
}

void BankCardPage::populateWidgets()
{
  if (entityId())
  {      
    // Contents page
    ui_.bankCodeEdit->setText(QString::number(entityData_.bankCode));
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

void BankCardPage::populateExportData()
{
  static QString bankCodeCaption   = tr("Bank code");
  static QString fullNameCaption   = tr("Full name");
  static QString shortNameCaption  = tr("Short name");
  static QString remarksCaption    = tr("Remarks");      
  static QString createUserCaption = tr("User, creator");
  static QString createTimeCaption = tr("Time, created");
  static QString updateUserCaption = tr("User, last update");
  static QString updateTimeCaption = tr("Time, last update");                    
        
  ExportDataList exportDataList;
  exportDataList << std::make_pair(
      bankCodeCaption, QVariant(entityData_.bankCode));
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

bool BankCardPage::updateDatabase()
{
  static const char* querySql = "update \"BANK\" set " \
      "\"BANK_CODE\" = ?, \"FULL_NAME\" = ?, \"SHORT_NAME\" = ?," \
      " \"REMARK\" = ?, \"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\' " \
      "where \"BANK_ID\" = ?";

  if (!entityId())
  {
    return false;
  }

  QSqlQuery query(databaseModel()->database());              
  if (!query.prepare(querySql))
  {
    return false;
  }

  query.bindValue(0, entityData_.bankCode);
  query.bindValue(1, entityData_.fullName);
  query.bindValue(2, entityData_.shortName);
  query.bindValue(3, buildQVariant(entityData_.remark));
  query.bindValue(4, databaseModel()->userId());
  query.bindValue(5, entityData_.bankId);
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

bool BankCardPage::insertIntoDatabase()
{
  static const char* querySql = "insert into \"BANK\"(" \
      "\"BANK_ID\", \"BANK_CODE\", \"FULL_NAME\", \"SHORT_NAME\"," \
      " \"REMARK\", \"CREATE_USER_ID\", \"UPDATE_USER_ID\"," \
      " \"CREATE_TIME\", \"UPDATE_TIME\") " \
      "values (?, ?, ?, ?, ?, ?, ?, \'NOW\', \'NOW\')";
      
  if (OptionalQInt64 newEntityId = databaseModel()->generateId(bankEntity))
  {
    entityData_.bankId = newEntityId.get();
    QSqlQuery query(databaseModel()->database());              
    if (!query.prepare(querySql))
    {
      return false;
    }

    query.bindValue(0, entityData_.bankId);
    query.bindValue(1, entityData_.bankCode);
    query.bindValue(2, entityData_.fullName);
    query.bindValue(3, entityData_.shortName);
    query.bindValue(4, buildQVariant(entityData_.remark));
    query.bindValue(5, databaseModel()->userId());
    query.bindValue(6, databaseModel()->userId());
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

bool BankCardPage::removeFromDatabase()
{
  static const char* querySql = "delete from \"BANK\" where \"BANK_ID\" = ?";

  if (QMessageBox::Yes == QMessageBox::question(this, tr("Banks"), 
      tr("Delete bank?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
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

    query.bindValue(0, entityData_.bankId);
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

void BankCardPage::on_lineEdit_textEdited(const QString&)
{
  contentsChanged_ = true;
  updateWidgets();
}

void BankCardPage::on_textEdit_textChanged()
{
  contentsChanged_ = true;
  updateWidgets();
}

void BankCardPage::showRequiredMarkers(bool visible)
{
  WidgetUtility::show(requiredMarkers_, visible);
}

void BankCardPage::on_createUserBtn_clicked(bool /*checked*/)
{
  if (entityId())
  {      
    showUserCardTaskWindow(entityData_.createUserId);
  }        
}

void BankCardPage::on_updateUserBtn_clicked(bool /*checked*/)
{
  if (entityId())
  {      
    showUserCardTaskWindow(entityData_.updateUserId);
  }        
}

void BankCardPage::on_database_userUpdated(const model::User& user)
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

} // namespace chrono
} // namespace ma

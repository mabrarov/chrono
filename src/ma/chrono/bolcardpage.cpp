/*
TRANSLATOR ma::chrono::BolCardPage
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
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
#include <ma/chrono/widgetutility.h>
#include <ma/chrono/listselectdialog.h>
#include <ma/chrono/ccdlistpage.h>
#include <ma/chrono/autoenterpriselistpage.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/ccd.h>
#include <ma/chrono/model/counterpart.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/autoenterprise.h>
#include <ma/chrono/localeutility.h>
#include <ma/chrono/bolcardpage.h>

namespace ma {
namespace chrono {

BolCardPage::BolCardPage(const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerWeakPtr& taskWindowManager,
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, Mode mode, 
    const OptionalQInt64& entityId, QWidget* parent)
  : CardPage(resourceManager, taskWindowManager, databaseModel, helpAction, 
        bolEntity, mode, entityId, parent)
  , dataAwareWidgets_()
  , contentsChanged_(false)     
{
  ui_.setupUi(this);          
  ui_.tabWidget->setCurrentIndex(0);
  setWindowIcon(resourceManager->getIcon(chronoIconName));
  LocaleUtility::setupLocaleAwareWidget(ui_.bolDate);
  LocaleUtility::setupLocaleAwareWidget(ui_.loadPermDate);
  ui_.placeCountSpin->setMaximum(model::Bol::placeCountMax);
  createAction()->setText(tr("&New bill of lading"));
  dataAwareWidgets_ << ui_.bolNumberEdit << ui_.bolDate 
                    << ui_.vehicleNumberEdit << ui_.loadPermDate 
                    << ui_.placeCountSpin << ui_.weightSpin
                    << ui_.volumeSpin << ui_.remarkTextEdit;
  requiredMarkers_  << ui_.bolNumberAsterikLabel << ui_.bolDateAsterikLabel
                    << ui_.ccdAsterikLabel << ui_.autoenterpriseAsterikLabel
                    << ui_.loadPermDateAsterikLabel 
                    << ui_.placeCountAsterikLabel << ui_.weightAsterikLabel 
                    << ui_.volumeAsterikLabel;
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
  QObject::connect(databaseModel.get(), 
      SIGNAL(userUpdated(const ma::chrono::model::User&)), 
      SLOT(on_database_userUpdated(const ma::chrono::model::User&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(ccdUpdated(const ma::chrono::model::Ccd&)), 
      SLOT(on_database_ccdUpdated(const ma::chrono::model::Ccd&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(ccdRemoved(const ma::chrono::model::Ccd&)), 
      SLOT(on_database_ccdRemoved(const ma::chrono::model::Ccd&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(autoenterpriseUpdated(const ma::chrono::model::Autoenterprise&)), 
      SLOT(on_database_autoenterpriseUpdated(
          const ma::chrono::model::Autoenterprise&)));
  QObject::connect(databaseModel.get(), 
      SIGNAL(autoenterpriseRemoved(const ma::chrono::model::Autoenterprise&)),
      SLOT(on_database_autoenterpriseRemoved(
          const ma::chrono::model::Autoenterprise&)));
  QObject::connect(databaseModel.get(),
      SIGNAL(counterpartUpdated(const ma::chrono::model::Counterpart&)),
      SLOT(on_database_counterpartUpdated(
          const ma::chrono::model::Counterpart&)));
  QObject::connect(databaseModel.get(),
      SIGNAL(contractUpdated(const ma::chrono::model::Contract&)),
      SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));
  updateWidgets();      
}

BolCardPage::~BolCardPage()
{
} 

void BolCardPage::refresh()
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

void BolCardPage::save()
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

void BolCardPage::cancel()
{
  if (editMode == mode())
  {      
    populateWidgets();
  }      
  CardPage::cancel();
  updateWidgets();
}

void BolCardPage::edit()
{
  CardPage::edit();
  updateWidgets();
}

void BolCardPage::remove()
{
  if (createMode != mode())
  {
    if (removeFromDatabase())
    {
      CardPage::remove();
    }
  }            
}

std::auto_ptr<CardPage> BolCardPage::createCardPage(CardPage::Mode mode,
    const OptionalQInt64& entityId) const
{
  std::auto_ptr<CardPage> cardPage(new BolCardPage(resourceManager(), 
      taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
  return cardPage;
}

void BolCardPage::connectDataAwareWidgets()
{
  typedef QWidgetList::const_iterator const_iterator;
      
  for (const_iterator it = dataAwareWidgets_.begin(),
      end = dataAwareWidgets_.end(); it != end; ++it)
  {
    QWidget* widget = *it;        
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
    else if (QSpinBox* edit = qobject_cast<QSpinBox*>(widget))
    {
      QObject::connect(edit, SIGNAL(valueChanged(const QString&)), 
          SLOT(on_lineEdit_textEdited(const QString&)));
    }        
    else if (QDateEdit* edit = qobject_cast<QDateEdit*>(widget))
    {
      QObject::connect(edit, SIGNAL(dateChanged(const QDate&)), 
          SLOT(on_dateEdit_dateChanged(const QDate&)));
    }
  }
}

void BolCardPage::updateWidgets()
{
  typedef QWidgetList::const_iterator const_iterator;

  Mode currentMode = mode();
  bool readOnly = viewMode == currentMode;
  for (const_iterator it = dataAwareWidgets_.begin(), 
      end = dataAwareWidgets_.end(); it != end; ++it)
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
  bool peristance = createMode != currentMode && entityId();      
  int generalTabIndex = ui_.tabWidget->indexOf(ui_.generalTab);      
  ui_.tabWidget->setTabEnabled(generalTabIndex, peristance);            
  switch (currentMode)
  {
  case viewMode:
    if (entityId())
    {
      updateWindowTitle(tr("Bills of lading - %1").arg(entityData_.bolNumber));
    }
    showRequiredMarkers(false);
    break;

  case editMode:
    if (entityId())
    {
      if (contentsChanged_)
      {
        updateWindowTitle(
            tr("Bills of lading - %1* - edit").arg(entityData_.bolNumber));
      }
      else
      {
        updateWindowTitle(
            tr("Bills of lading - %1 - edit").arg(entityData_.bolNumber));
      }        
    }
    showRequiredMarkers(true);
    break;

  case createMode:
    updateWindowTitle(tr("Bills of lading - New bill of lading creation*"));
    showRequiredMarkers(true);
    break;
  }
}

bool BolCardPage::populateFromWidgets()
{
  static const char* bolNumberTitle = QT_TR_NOOP("Number of bill of lading");
  static const char* vehicleNumberTitle  = QT_TR_NOOP("Vehicle number");
  static const char* ccdTitle            = QT_TR_NOOP("CCD");
  static const char* autoenterpriseTitle = QT_TR_NOOP("Autoenterprise");
  static const char* remarkTitle         = QT_TR_NOOP("Remark");
      
  static const char* messageTitle = QT_TR_NOOP("Invalid data input");
  static const char* notEmpty     = QT_TR_NOOP("Field \"%1\" can\'t be empty.");
  static const char* tooLongText  = 
      QT_TR_NOOP("Field \"%1\" can\'t exceed %2 characters.");
            
  int contentsTabIndex = ui_.tabWidget->indexOf(ui_.contentsTab);
  bool validated;
  // Get BOL number
  QString bolNumber = ui_.bolNumberEdit->displayText();            
  if (bolNumber.isEmpty())
  {
    validated = false;
    QMessageBox::warning(this, tr(messageTitle), 
        tr(notEmpty).arg(tr(bolNumberTitle)));
  } 
  else if (bolNumber.length() > model::Bol::bolNumberMaxLen)
  {
    validated = false;
    QMessageBox::warning(this, tr(messageTitle), 
        tr(tooLongText).arg(tr(bolNumberTitle))
            .arg(model::Bol::bolNumberMaxLen));
  }
  else
  {
    validated = true;
  }
  if (!validated)
  {
    ui_.tabWidget->setCurrentIndex(contentsTabIndex);
    ui_.bolNumberEdit->setFocus(Qt::TabFocusReason);
    ui_.bolNumberEdit->selectAll();
    return false;
  } 
  // Get CCD
  if (!selectedCcdId_)
  {        
    QMessageBox::warning(this, tr(messageTitle), 
        tr(notEmpty).arg(tr(ccdTitle)));
    ui_.tabWidget->setCurrentIndex(contentsTabIndex);
    ui_.ccdBtn->setFocus(Qt::TabFocusReason);
    return false;
  }
  // Get autoenterprise
  if (!selectedAutoentrpriseId_)
  {        
    QMessageBox::warning(this, tr(messageTitle), 
        tr(notEmpty).arg(tr(autoenterpriseTitle)));
    ui_.tabWidget->setCurrentIndex(contentsTabIndex);
    ui_.autoenterpriseBtn->setFocus(Qt::TabFocusReason);
    return false;
  }
  // Get vehicle number
  QString vehicleNumber = ui_.vehicleNumberEdit->displayText();            
  if (vehicleNumber.length() > model::Bol::vehicleNumberMaxLen)
  {        
    QMessageBox::warning(this, tr(messageTitle), 
        tr(tooLongText).arg(tr(vehicleNumberTitle))
            .arg(model::Bol::vehicleNumberMaxLen));
    ui_.tabWidget->setCurrentIndex(contentsTabIndex);
    ui_.vehicleNumberEdit->setFocus(Qt::TabFocusReason);
    ui_.vehicleNumberEdit->selectAll();
    return false;
  }      
  // Get remark      
  QString remark = ui_.remarkTextEdit->toPlainText().trimmed();
  if (remark.length() > model::Bol::remarkMaxLen)
  {
    QMessageBox::warning(this, tr(messageTitle), 
        tr(tooLongText).arg(tr(remarkTitle)).arg(model::Bol::remarkMaxLen));
    ui_.tabWidget->setCurrentIndex(contentsTabIndex);
    ui_.remarkTextEdit->setFocus(Qt::TabFocusReason);
    ui_.remarkTextEdit->selectAll();
    return false;
  }
                      
  entityData_.bolNumber = bolNumber;
  entityData_.ccdId     = selectedCcdId_.get();
  entityData_.autoenterpriseId = selectedAutoentrpriseId_.get();
  entityData_.vehicleNumber = 
      vehicleNumber.isEmpty() ? OptionalQString() : vehicleNumber;
  entityData_.bolDate   = ui_.bolDate->date();
  entityData_.loadPermDate = ui_.loadPermDate->date();
  entityData_.placeCount = ui_.placeCountSpin->value();
  entityData_.cargoWeight = ui_.weightSpin->value();
  entityData_.cargoVolume = ui_.volumeSpin->value();
  entityData_.remark = remark.isEmpty() ? OptionalQString() : remark;      
  return true;
}

bool BolCardPage::populateFromDatabase()
{      
  static const char* querySql = "select " \
      "t.\"NUMBER\", t.\"BOL_DATE\", t.\"CCD_ID\", ccd.\"NUMBER\", " \
      "ccd.\"CONTRACT_ID\", " \
      "cntp.\"SHORT_NAME\", " \
      "a.\"SHORT_NAME\", t.\"VEHICLE_NUMBER\", t.\"AUTOENTERPRISE_ID\", " \
      "t.\"PLACE_COUNT\", t.\"CARGO_WEIGHT\", t.\"CARGO_VOLUME\", " \
      "t.\"LOAD_PERM_DATE\", t.\"REMARK\", t.\"CREATE_USER_ID\", " \
      "cu.\"LOGIN\", t.\"UPDATE_USER_ID\", uu.\"LOGIN\", " \
      "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", " \
      "cr.\"COUNTERPART_ID\" " \
      "from \"BOL\" t " \
      "join \"CCD\" ccd on t.\"CCD_ID\" = ccd.\"CCD_ID\" " \
      "join \"CONTRACT\" cr on ccd.\"CONTRACT_ID\" = cr.\"CONTRACT_ID\" " \
      "join \"COUNTERPART\" cntp on cr.\"COUNTERPART_ID\" = " \
      "cntp.\"COUNTERPART_ID\" " \
      "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
      "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
      "join \"AUTOENTERPRISE\" a on t.\"AUTOENTERPRISE_ID\" = " \
      "a.\"AUTOENTERPRISE_ID\" " \
      "where t.\"BOL_ID\" = ?";

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
  
  entityData_.bolId   = entityId().get();
  entityData_.bolNumber = query.value(0).toString();
  entityData_.bolDate   = databaseModel()->convertFromServer(
      query.value(1).toDate());
  entityData_.ccdId     = query.value(2).toLongLong();
  ccdNumber_            = query.value(3).toString();
  contractId_           = query.value(4).toLongLong();
  counterpartShortName_     = query.value(5).toString();
  autoenterpriseShortName_  = query.value(6).toString();
  entityData_.vehicleNumber = 
      query.isNull(7) ? OptionalQString() : query.value(7).toString();
  entityData_.autoenterpriseId = query.value(8).toLongLong();
  entityData_.placeCount  = query.value(9).toLongLong();
  entityData_.cargoWeight = query.value(10).toLongLong();
  entityData_.cargoVolume = query.value(11).toDouble();
  entityData_.loadPermDate = databaseModel()->convertFromServer(
      query.value(12).toDate());
  entityData_.remark = 
      query.isNull(13) ? OptionalQString() : query.value(13).toString();
  entityData_.createUserId = query.value(14).toLongLong();
  createUserName_          = query.value(15).toString(); 
  entityData_.updateUserId = query.value(16).toLongLong();
  updateUserName_          = query.value(17).toString(); 
  entityData_.createTime   = databaseModel()->convertFromServer(
      query.value(18).toDateTime()); 
  entityData_.updateTime   = databaseModel()->convertFromServer(
      query.value(19).toDateTime());
  counterpartId_ = query.value(20).toLongLong();              
  return true;
}

void BolCardPage::populateWidgets()
{
  if (entityId())
  { 
    // Contents page
    ui_.bolNumberEdit->setText(entityData_.bolNumber);      
    ui_.bolDate->setDate(entityData_.bolDate);
    selectedContractId_ = contractId_;
    selectedCcdId_ = entityData_.ccdId;
    ui_.ccdEdit->setText(ccdNumber_);
    selectedCounterpartId_ = counterpartId_;
    ui_.counterpartEdit->setText(counterpartShortName_);
    selectedAutoentrpriseId_ = entityData_.autoenterpriseId;
    ui_.autoenterpriseEdit->setText(autoenterpriseShortName_);        
    ui_.vehicleNumberEdit->setText(entityData_.vehicleNumber 
        ? entityData_.vehicleNumber.get() : QString());
    ui_.loadPermDate->setDate(entityData_.loadPermDate);
    ui_.placeCountSpin->setValue(entityData_.placeCount);
    ui_.weightSpin->setValue(entityData_.cargoWeight);
    ui_.volumeSpin->setValue(entityData_.cargoVolume);      
    ui_.remarkTextEdit->setPlainText(
        entityData_.remark ? entityData_.remark.get() : QString());
    // General data page
    ui_.createUserEdit->setText(createUserName_);
    ui_.createTimeEdit->setText(
        entityData_.createTime.toString(Qt::DefaultLocaleShortDate));
    ui_.updateUserEdit->setText(updateUserName_);
    ui_.updateTimeEdit->setText(
        entityData_.updateTime.toString(Qt::DefaultLocaleShortDate));
    populateExportData();
    contentsChanged_ = false;
  }
} 

void BolCardPage::populateExportData()
{
  static const QString bolNumberCaption = tr("BOL No.");
  static const QString bolDateCaption = tr("BOL Date");
  static const QString ccdNumberCaption = tr("CCD");
  static const QString counterpartShortNameCaption = tr("Counterpart");
  static const QString autoenterpriseShortNameCaption = tr("Autoenterprise");
  static const QString vehicleNumberCaption = tr("Vehicle No.");
  static const QString loadPermDateCaption = tr("Load permission date");
  static const QString placeCountCaption = tr("Place count");
  static const QString weightCaption = tr("Weight brutto, kg");
  static const QString volumeCaption = tr("Volume");
  static const QString remarksCaption = tr("Remarks");      
  static const QString createUserCaption = tr("User, creator");
  static const QString createTimeCaption = tr("Time, created");
  static const QString updateUserCaption = tr("User, last update");
  static const QString updateTimeCaption = tr("Time, last update");                    
        
  ExportDataList exportDataList;
  exportDataList << std::make_pair(bolNumberCaption, QVariant(entityData_.bolNumber));
  exportDataList << std::make_pair(bolDateCaption, QVariant(entityData_.bolDate));
  exportDataList << std::make_pair(ccdNumberCaption, QVariant(ccdNumber_));
  exportDataList << std::make_pair(counterpartShortNameCaption, QVariant(counterpartShortName_));
  exportDataList << std::make_pair(autoenterpriseShortNameCaption, QVariant(autoenterpriseShortName_));
  exportDataList << std::make_pair(vehicleNumberCaption, QVariant(entityData_.vehicleNumber ? entityData_.vehicleNumber.get() : QString()));
  exportDataList << std::make_pair(loadPermDateCaption, QVariant(entityData_.loadPermDate));
  exportDataList << std::make_pair(placeCountCaption, QVariant(entityData_.placeCount));
  exportDataList << std::make_pair(weightCaption, QVariant(entityData_.cargoWeight));
  exportDataList << std::make_pair(volumeCaption, QVariant(entityData_.cargoVolume));
  exportDataList << std::make_pair(remarksCaption, entityData_.remark ? QVariant(entityData_.remark.get()) : QVariant(QVariant::String));
  exportDataList << std::make_pair(createUserCaption, QVariant(createUserName_));
  exportDataList << std::make_pair(createTimeCaption, QVariant(entityData_.createTime));
  exportDataList << std::make_pair(updateUserCaption, QVariant(updateUserName_));
  exportDataList << std::make_pair(updateTimeCaption, QVariant(entityData_.updateTime));      

  setExportDataList(exportDataList);
}

bool BolCardPage::updateDatabase()
{
  static const char* querySql = "update \"BOL\" set " \
    "\"VEHICLE_NUMBER\" = ?, \"NUMBER\" = ?, \"BOL_DATE\" = ?, " \
    "\"AUTOENTERPRISE_ID\" = ?, \"PLACE_COUNT\" = ?, \"CARGO_WEIGHT\" = ?, " \
    "\"LOAD_PERM_DATE\" = ?, \"CARGO_VOLUME\" = ?, \"REMARK\" = ?, \"CCD_ID\" = ?, " \
    "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\' " \
    "where \"BOL_ID\" = ?";

  if (entityId())
  {        
    QSqlQuery query(databaseModel()->database());              
    if (query.prepare(querySql))
    {          
      query.bindValue(0, entityData_.vehicleNumber ? entityData_.vehicleNumber.get() : QVariant(QVariant::String));
      query.bindValue(1, entityData_.bolNumber);
      query.bindValue(2, entityData_.bolDate);
      query.bindValue(3, entityData_.autoenterpriseId);
      query.bindValue(4, entityData_.placeCount);
      query.bindValue(5, entityData_.cargoWeight);
      query.bindValue(6, entityData_.loadPermDate);
      query.bindValue(7, entityData_.cargoVolume);
      query.bindValue(8, entityData_.remark ? entityData_.remark.get() : QVariant(QVariant::String));
      query.bindValue(9, entityData_.ccdId);
      query.bindValue(10, databaseModel()->userId());
      query.bindValue(11, entityData_.bolId);
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

bool BolCardPage::insertIntoDatabase()
{
  static const char* querySql = "insert into \"BOL\" (" \
    "\"BOL_ID\", \"VEHICLE_NUMBER\", \"NUMBER\", \"BOL_DATE\", " \
    "\"AUTOENTERPRISE_ID\", \"PLACE_COUNT\", " \
    "\"CARGO_WEIGHT\", \"LOAD_PERM_DATE\", " \
    "\"CARGO_VOLUME\", \"REMARK\", \"CCD_ID\", " \
    "\"CREATE_USER_ID\", \"UPDATE_USER_ID\", \"CREATE_TIME\", \"UPDATE_TIME\") " \
    "values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, \'NOW\', \'NOW\')";      

  if (OptionalQInt64 newEntityId = databaseModel()->generateId(bolEntity))
  {
    entityData_.bolId = newEntityId.get();    
    QSqlQuery query(databaseModel()->database());              
    if (query.prepare(querySql))
    {
      query.bindValue(0, entityData_.bolId);
      query.bindValue(1, entityData_.vehicleNumber ? entityData_.vehicleNumber.get() : QVariant(QVariant::String));
      query.bindValue(2, entityData_.bolNumber);
      query.bindValue(3, entityData_.bolDate);
      query.bindValue(4, entityData_.autoenterpriseId);
      query.bindValue(5, entityData_.placeCount);
      query.bindValue(6, entityData_.cargoWeight);
      query.bindValue(7, entityData_.loadPermDate);
      query.bindValue(8, entityData_.cargoVolume);
      query.bindValue(9, entityData_.remark ? entityData_.remark.get() : QVariant(QVariant::String));
      query.bindValue(10, entityData_.ccdId);
      query.bindValue(11, databaseModel()->userId());
      query.bindValue(12, databaseModel()->userId());      
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

bool BolCardPage::removeFromDatabase()
{
  static const char* querySql = "delete from \"BOL\" where \"BOL_ID\" = ?";

  if (QMessageBox::Yes == QMessageBox::question(this, tr("Bills of lading"), 
    tr("Delete bill of lading?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
  {
    if (entityId())
    {
      QSqlQuery query(databaseModel()->database());              
      if (query.prepare(querySql))
      {
        query.bindValue(0, entityData_.bolId);
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

void BolCardPage::on_lineEdit_textEdited(const QString&)
{
  contentsChanged_ = true;
  updateWidgets();
}

void BolCardPage::on_textEdit_textChanged()
{
  contentsChanged_ = true;
  updateWidgets();
}

void BolCardPage::on_dateEdit_dateChanged(const QDate&)
{
  contentsChanged_ = true;
  updateWidgets();
}    

void BolCardPage::on_ccdBtn_clicked(bool /*checked*/)
{
  if (viewMode == mode())
  {
    if (entityId())
    {          
      showCcdCardTaskWindow(entityData_.ccdId);
    }        
  }
  else
  {
    //todo: add default selection
    std::auto_ptr<ListPage> listPage(new CcdListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), 
      OptionalInt(), UserDefinedFilter(), UserDefinedOrder(), 0));
    ListSelectDialog listSelectDialog(this, listPage);
    listSelectDialog.setWindowModality(Qt::WindowModal);
    if (QDialog::Accepted == listSelectDialog.exec())
    {          
      selectedCcdId_ = listSelectDialog.listPage()->selectedId();          
      if (OptionalQString ccdNumber = listSelectDialog.listPage()->selectedRelationalText())
      {
        ui_.ccdEdit->setText(ccdNumber.get());
      }          
      updateCcdRelatedNonPersistData(selectedCcdId_.get());
      contentsChanged_ = true;
      updateWidgets();
    }
  } 
}
    
void BolCardPage::on_counterpartBtn_clicked(bool /*checked*/)
{
  if (selectedCounterpartId_)
  {
    showCounterpartCardTaskWindow(selectedCounterpartId_.get());
  }
}      

void BolCardPage::on_autoenterpriseBtn_clicked(bool /*checked*/)
{
  if (viewMode == mode())
  {
    if (entityId())
    {          
      showAutoenterpriseCardTaskWindow(entityData_.autoenterpriseId);
    }        
  }
  else
  {
    //todo: add default selection
    std::auto_ptr<ListPage> listPage(new AutoenterpriseListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), 
      UserDefinedFilter(), UserDefinedOrder(), 0));
    ListSelectDialog listSelectDialog(this, listPage);
    listSelectDialog.setWindowModality(Qt::WindowModal);
    if (QDialog::Accepted == listSelectDialog.exec())
    {          
      selectedAutoentrpriseId_ = listSelectDialog.listPage()->selectedId();          
      if (OptionalQString autoenterpriseShortName = listSelectDialog.listPage()->selectedRelationalText())
      {
        ui_.autoenterpriseEdit->setText(autoenterpriseShortName.get());
      }
      contentsChanged_ = true;
      updateWidgets();
    }
  } 
}

void BolCardPage::updateCcdRelatedPersistData(qint64 ccdId)
{
  static const char* querySql = "select " \
    "cn.\"COUNTERPART_ID\", cnt.\"SHORT_NAME\", cn.\"CONTRACT_ID\", ccd.\"NUMBER\" " \
    "from \"CCD\" ccd " \
    "join \"CONTRACT\" cn on ccd.\"CONTRACT_ID\" = cn.\"CONTRACT_ID\" " \
    "join \"COUNTERPART\" cnt on cn.\"COUNTERPART_ID\" = cnt.\"COUNTERPART_ID\" " \
    "where ccd.\"CCD_ID\" = ?";
      
  if (entityId())
  {
    QSqlQuery query(databaseModel()->database());              
    if (query.prepare(querySql))
    {          
      query.bindValue(0, ccdId);          
      if (query.exec())
      {
        if (query.next())
        {   
          ccdNumber_ = query.value(3).toString();
          contractId_ = query.value(2).toLongLong();              
          counterpartId_ = query.value(0).toLongLong();
          counterpartShortName_ = query.value(1).toString();
        }
      }
    }            
  }
}

void BolCardPage::updateCcdRelatedNonPersistData(qint64 ccdId)
{
  static const char* querySql = "select " \
    "cn.\"COUNTERPART_ID\", cnt.\"SHORT_NAME\", cn.\"CONTRACT_ID\", ccd.\"NUMBER\" " \
    "from \"CCD\" ccd " \
    "join \"CONTRACT\" cn on ccd.\"CONTRACT_ID\" = cn.\"CONTRACT_ID\" " \
    "join \"COUNTERPART\" cnt on cn.\"COUNTERPART_ID\" = cnt.\"COUNTERPART_ID\" " \
    "where ccd.\"CCD_ID\" = ?";
      
  if (selectedCcdId_)
  {
    QSqlQuery query(databaseModel()->database());              
    if (query.prepare(querySql))
    {          
      query.bindValue(0, ccdId);          
      if (query.exec())
      {
        if (query.next())
        {   
          ui_.ccdEdit->setText(query.value(3).toString());
          selectedContractId_ = query.value(2).toLongLong();              
          selectedCounterpartId_ = query.value(0).toLongLong();
          ui_.counterpartEdit->setText(query.value(1).toString());
        }
      }
    }            
  }
}

void BolCardPage::showRequiredMarkers(bool visible)
{
  WidgetUtility::show(requiredMarkers_, visible);
}

void BolCardPage::on_createUserBtn_clicked(bool /*checked*/)
{
  if (entityId())
  {      
    showUserCardTaskWindow(entityData_.createUserId);
  }        
}

void BolCardPage::on_updateUserBtn_clicked(bool /*checked*/)
{
  if (entityId())
  {      
    showUserCardTaskWindow(entityData_.updateUserId);
  }        
}

void BolCardPage::on_database_userUpdated(const ma::chrono::model::User& user)
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

void BolCardPage::on_database_ccdUpdated(const ma::chrono::model::Ccd& ccd)
{
  if (entityId())
  {        
    if (entityData_.ccdId == ccd.ccdId)
    {
      updateCcdRelatedPersistData(ccd.ccdId);
    }
  }
  if (selectedCcdId_)
  {
    if (selectedCcdId_.get() == ccd.ccdId)
    {
      if (selectedCcdId_.get() == entityData_.ccdId)
      {
        ui_.ccdEdit->setText(ccdNumber_);
        selectedContractId_ = contractId_;              
        selectedCounterpartId_ = counterpartId_;
        ui_.counterpartEdit->setText(counterpartShortName_);         
      }
      else
      {
        updateCcdRelatedNonPersistData(ccd.ccdId);
      }
      updateWidgets();
    }
  }      
}

void BolCardPage::on_database_ccdRemoved(const ma::chrono::model::Ccd& ccd)
{
  if (selectedCcdId_)
  {
    if (selectedCcdId_.get() == ccd.ccdId)
    {
      ui_.ccdEdit->setText(QString());
      selectedContractId_.reset();              
      selectedCounterpartId_.reset();
      ui_.counterpartEdit->setText(QString());
      updateWidgets();
    }
  }
}

void BolCardPage::on_database_autoenterpriseUpdated(const ma::chrono::model::Autoenterprise& autoenterprise)
{
  if (entityId())
  {
    if (entityData_.autoenterpriseId == autoenterprise.autoenterpriseId)
    {          
      autoenterpriseShortName_ = autoenterprise.shortName;
    }
  }
  if (selectedAutoentrpriseId_)
  {
    if (selectedAutoentrpriseId_.get() == autoenterprise.autoenterpriseId)
    {
      ui_.autoenterpriseEdit->setText(autoenterprise.shortName);          
      updateWidgets();
    }
  }
}

void BolCardPage::on_database_autoenterpriseRemoved(const ma::chrono::model::Autoenterprise& autoenterprise)
{
  if (selectedAutoentrpriseId_)
  {
    if (selectedAutoentrpriseId_.get() == autoenterprise.autoenterpriseId)
    {
      ui_.autoenterpriseEdit->setText(QString());
      selectedAutoentrpriseId_.reset();
      updateWidgets();
    }
  }
}

void BolCardPage::on_database_counterpartUpdated(const ma::chrono::model::Counterpart& counterpart)
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

void BolCardPage::on_database_contractUpdated(const ma::chrono::model::Contract& contract)
{
  if (entityId())
  {        
    if (contractId_ == contract.contractId)
    {
      updateCcdRelatedPersistData(entityData_.ccdId);
    }
  }
  if (selectedContractId_)
  {
    if (selectedContractId_.get() == contract.contractId)
    {
      if (selectedContractId_.get() == contractId_)
      {
        ui_.ccdEdit->setText(ccdNumber_);
        selectedContractId_ = contractId_;              
        selectedCounterpartId_ = counterpartId_;
        ui_.counterpartEdit->setText(counterpartShortName_);         
      }
      else
      {
        updateCcdRelatedNonPersistData(selectedCcdId_.get());
      }
      updateWidgets();
    }
  }
}

} // namespace chrono
} // namespace ma

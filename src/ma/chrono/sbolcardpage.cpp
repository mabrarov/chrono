/*
TRANSLATOR ma::chrono::SbolCardPage
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <QAction>
//#include <QPalette>
#include <QMessageBox>
#include <QCalendarWidget>
#include <QSqlQuery>
#include <QSqlError>
#include <ma/chrono/constants.h>
#include <ma/chrono/dbconstants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/widgetutility.h>
#include <ma/chrono/sqlutility.h>
#include <ma/chrono/listselectdialog.h>
#include <ma/chrono/ccdlistpage.h>
#include <ma/chrono/lcreditlistpage.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/ccd.h>
#include <ma/chrono/model/counterpart.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/lcredit.h>
#include <ma/chrono/localeutility.h>
#include <ma/chrono/sbolcardpage.h>

namespace ma
{
  namespace chrono
  {    
    SbolCardPage::SbolCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent)
      : CardPage(resourceManager, taskWindowManager, databaseModel, helpAction, sbolEntity, mode, entityId, parent)
      , dataAwareWidgets_()
      , contentsChanged_(false)     
    {
      ui_.setupUi(this);          
      ui_.tabWidget->setCurrentIndex(0);
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      LocaleUtility::setupLocaleAwareWidget(ui_.sbolDate);
      createAction()->setText(tr("&New shipped bill of lading"));
      dataAwareWidgets_ << ui_.sbolNumberEdit << ui_.sbolDate << ui_.remarkTextEdit;
      requiredMarkers_  << ui_.asterikLabel << ui_.asterikLabel_2 << ui_.asterikLabel_3; 
      ui_.removeLcreditBtn->setIcon(resourceManager->getIcon(clearEditIconName));
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
      QObject::connect(databaseModel.get(), SIGNAL(ccdUpdated(const ma::chrono::model::Ccd&)), SLOT(on_database_ccdUpdated(const ma::chrono::model::Ccd&)));
      QObject::connect(databaseModel.get(), SIGNAL(ccdRemoved(const ma::chrono::model::Ccd&)), SLOT(on_database_ccdRemoved(const ma::chrono::model::Ccd&)));
      QObject::connect(databaseModel.get(), SIGNAL(lcreditUpdated(const ma::chrono::model::Lcredit&)), SLOT(on_database_lcreditUpdated(const ma::chrono::model::Lcredit&)));
      QObject::connect(databaseModel.get(), SIGNAL(lcreditRemoved(const ma::chrono::model::Lcredit&)), SLOT(on_database_lcreditRemoved(const ma::chrono::model::Lcredit&)));      
      QObject::connect(databaseModel.get(), SIGNAL(contractUpdated(const ma::chrono::model::Contract&)), SLOT(on_database_contractUpdated(const ma::chrono::model::Contract&)));
      QObject::connect(databaseModel.get(), SIGNAL(counterpartUpdated(const ma::chrono::model::Counterpart&)), SLOT(on_database_counterpartUpdated(const ma::chrono::model::Counterpart&)));                  
      updateWidgets();      
    }

    SbolCardPage::~SbolCardPage()
    {
    } 

    void SbolCardPage::refresh()
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

    void SbolCardPage::save()
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

    void SbolCardPage::cancel()
    {
      if (editMode == mode())
      {      
        populateWidgets();
      }      
      CardPage::cancel();
      updateWidgets();
    }

    void SbolCardPage::edit()
    {      
      CardPage::edit();
      updateWidgets();
    }

    void SbolCardPage::remove()
    {
      if (createMode != mode())
      {
        if (removeFromDatabase())
        {
          CardPage::remove();
        }
      }            
    }

    std::auto_ptr<CardPage> SbolCardPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new SbolCardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), mode, entityId));
      return cardPage;
    }

    void SbolCardPage::connectDataAwareWidgets()
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
        else if (QDateEdit* edit = qobject_cast<QDateEdit*>(widget))
        {
          QObject::connect(edit, SIGNAL(dateChanged(const QDate&)), SLOT(on_dateEdit_dateChanged(const QDate&)));
        }
      }
    }

    void SbolCardPage::updateWidgets()
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
      bool peristance = createMode != currentMode && entityId();      
      int generalTabIndex = ui_.tabWidget->indexOf(ui_.generalTab);      
      ui_.tabWidget->setTabEnabled(generalTabIndex, peristance);            
      ui_.lcreditBtn->setEnabled(viewMode != currentMode || selectedLcreditId_);
      ui_.removeLcreditBtn->setVisible(viewMode != currentMode && selectedLcreditId_);
      switch (currentMode)
      {
      case viewMode:
        if (entityId())
        {
          updateWindowTitle(tr("Shipped bills of lading - %1").arg(entityData_.sbolNumber));
        }
        showRequiredMarkers(false);
        break;

      case editMode:
        if (entityId())
        {
          if (contentsChanged_)
          {
            updateWindowTitle(tr("Shipped bills of lading - %1* - edit").arg(entityData_.sbolNumber));
          }
          else
          {
            updateWindowTitle(tr("Shipped bills of lading - %1 - edit").arg(entityData_.sbolNumber));
          }        
        }
        showRequiredMarkers(true);
        break;

      case createMode:
        updateWindowTitle(tr("Shipped bills of lading - New shipped bill of lading creation*"));
        showRequiredMarkers(true);
        break;
      }
    }

    bool SbolCardPage::populateFromWidgets()
    {
      static const char* sbolNumberTitle = QT_TR_NOOP("Number of shipped bill of lading");            
      static const char* ccdTitle = QT_TR_NOOP("CCD");
      static const char* lcreditTitle = QT_TR_NOOP("Letter of credit");      
      static const char* remarkTitle = QT_TR_NOOP("Remark");      
      
      static const char* messageTitle = QT_TR_NOOP("Invalid data input");
      static const char* notEmpty = QT_TR_NOOP("Field \"%1\" can\'t be empty.");
      static const char* tooLongText = QT_TR_NOOP("Field \"%1\" can\'t exceed %2 characters.");      
            
      int contentsTabIndex = ui_.tabWidget->indexOf(ui_.contentsTab);
      bool validated;
      // Get SBOL number
      QString sbolNumber = ui_.sbolNumberEdit->displayText();            
      if (sbolNumber.isEmpty())
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(sbolNumberTitle)));
      } 
      else if (sbolNumber.length() > model::Sbol::sbolNumberMaxLen)
      {
        validated = false;
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(sbolNumberTitle)).arg(model::Sbol::sbolNumberMaxLen));
      }
      else
      {
        validated = true;
      }
      if (!validated)
      {
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.sbolNumberEdit->setFocus(Qt::TabFocusReason);
        ui_.sbolNumberEdit->selectAll();
        return false;
      } 
      // Get CCD
      if (!selectedCcdId_)
      {        
        QMessageBox::warning(this, tr(messageTitle), tr(notEmpty).arg(tr(ccdTitle)));
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.ccdBtn->setFocus(Qt::TabFocusReason);
        return false;
      }      
      // Get remark      
      QString remark = ui_.remarkTextEdit->toPlainText().trimmed();
      if (remark.length() > model::Sbol::remarkMaxLen)
      {
        QMessageBox::warning(this, tr(messageTitle), tr(tooLongText).arg(tr(remarkTitle)).arg(model::Sbol::remarkMaxLen));        
        ui_.tabWidget->setCurrentIndex(contentsTabIndex);
        ui_.remarkTextEdit->setFocus(Qt::TabFocusReason);
        ui_.remarkTextEdit->selectAll();
        return false;
      }
                      
      entityData_.sbolNumber = sbolNumber;
      entityData_.ccdId      = selectedCcdId_.get();
      entityData_.lcreditId  = selectedLcreditId_;      
      entityData_.sbolDate   = ui_.sbolDate->date();      
      entityData_.remark     = remark.isEmpty() ? OptionalQString() : remark;      
      return true;      
    }

    bool SbolCardPage::populateFromDatabase()
    {      
      static const char* querySql = "select " \
        "ccd.\"CONTRACT_ID\", cr.\"COUNTERPART_ID\", cntp.\"SHORT_NAME\", " \
        "t.\"NUMBER\", t.\"SBOL_DATE\", t.\"CCD_ID\", t.\"LCREDIT_ID\", ccd.\"NUMBER\", l.\"NUMBER\", " \
        "t.\"REMARK\", " \
        "t.\"CREATE_USER_ID\", cu.\"LOGIN\", t.\"UPDATE_USER_ID\", uu.\"LOGIN\", " \
        "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
        "from \"SBOL\" t " \
        "join \"CCD\" ccd on t.\"CCD_ID\" = ccd.\"CCD_ID\" " \
        "join \"CONTRACT\" cr on ccd.\"CONTRACT_ID\" = cr.\"CONTRACT_ID\" " \
        "join \"COUNTERPART\" cntp on cr.\"COUNTERPART_ID\" = cntp.\"COUNTERPART_ID\" " \
        "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
        "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
        "left join \"LCREDIT\" l on t.\"LCREDIT_ID\" = l.\"LCREDIT_ID\" " \
        "where t.\"SBOL_ID\" = ?";      

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
              entityData_.sbolId = entityId().get();              
              contractId_    = query.value(0).toLongLong();
              counterpartId_ = query.value(1).toLongLong();
              counterpartShortName_  = query.value(2).toString();
              entityData_.sbolNumber = query.value(3).toString();
              entityData_.sbolDate   = databaseModel()->convertFromServer(query.value(4).toDate());
              entityData_.ccdId      = query.value(5).toLongLong();
              entityData_.lcreditId  = query.value(6).isNull() ? OptionalQInt64() : query.value(6).toLongLong();
              ccdNumber_     = query.value(7).toString();
              lcreditNumber_ = query.value(6).isNull() ? OptionalQString() : query.value(8).toString();
              entityData_.remark = query.isNull(9) ? OptionalQString() : query.value(9).toString();
              entityData_.createUserId = query.value(10).toLongLong();
              createUserName_          = query.value(11).toString(); 
              entityData_.updateUserId = query.value(12).toLongLong();
              updateUserName_          = query.value(13).toString(); 
              entityData_.createTime   = databaseModel()->convertFromServer(query.value(14).toDateTime()); 
              entityData_.updateTime   = databaseModel()->convertFromServer(query.value(15).toDateTime());
              return true;
            }
          }
        }
      }
      return false;
    }

    void SbolCardPage::populateWidgets()
    {
      if (entityId())
      { 
        // Contents page
        ui_.sbolNumberEdit->setText(entityData_.sbolNumber);      
        ui_.sbolDate->setDate(entityData_.sbolDate);
        selectedContractId_ = contractId_;
        selectedCcdId_ = entityData_.ccdId;
        ui_.ccdEdit->setText(ccdNumber_);
        selectedCounterpartId_ = counterpartId_;
        ui_.counterpartEdit->setText(counterpartShortName_);
        selectedLcreditId_ = entityData_.lcreditId;
        ui_.lcreditEdit->setText(lcreditNumber_ ? lcreditNumber_.get() : QString());                
        ui_.remarkTextEdit->setPlainText(entityData_.remark ? entityData_.remark.get() : QString());        
        // General data page
        ui_.createUserEdit->setText(createUserName_);
        ui_.createTimeEdit->setText(entityData_.createTime.toString(Qt::DefaultLocaleShortDate));
        ui_.updateUserEdit->setText(updateUserName_);
        ui_.updateTimeEdit->setText(entityData_.updateTime.toString(Qt::DefaultLocaleShortDate));
        populateExportData();
        contentsChanged_ = false;
      }
    }  

    void SbolCardPage::populateExportData()
    {
      static QString sbolNumberCaption = tr("Number of shipped bill of lading");
      static QString sbolDateCaption = tr("Date of shipped bill of lading");
      static QString ccdNumberCaption = tr("CCD number");      
      static QString counterpartShortNameCaption = tr("Counterpart");      
      static QString lcreditNumberCaption = tr("Letter of credit");      
      static QString remarksCaption = tr("Remarks");      
      static QString createUserCaption = tr("User, creator");
      static QString createTimeCaption = tr("Time, created");
      static QString updateUserCaption = tr("User, last update");
      static QString updateTimeCaption = tr("Time, last update");                    
        
      ExportDataList exportDataList;      
      exportDataList << std::make_pair(sbolNumberCaption , QVariant(entityData_.sbolNumber));
      exportDataList << std::make_pair(sbolDateCaption, QVariant(entityData_.sbolDate));
      exportDataList << std::make_pair(ccdNumberCaption, QVariant(ccdNumber_));
      exportDataList << std::make_pair(counterpartShortNameCaption, QVariant(counterpartShortName_));
      exportDataList << std::make_pair(lcreditNumberCaption, lcreditNumber_ ? QVariant(lcreditNumber_.get()) : QVariant(QVariant::String));           
      exportDataList << std::make_pair(remarksCaption, entityData_.remark ? QVariant(entityData_.remark.get()) : QVariant(QVariant::String));      
      exportDataList << std::make_pair(createUserCaption, QVariant(createUserName_));
      exportDataList << std::make_pair(createTimeCaption, QVariant(entityData_.createTime));
      exportDataList << std::make_pair(updateUserCaption, QVariant(updateUserName_));
      exportDataList << std::make_pair(updateTimeCaption, QVariant(entityData_.updateTime));      

      setExportDataList(exportDataList);
    }

    bool SbolCardPage::updateDatabase()
    {
      static const char* querySql = "update \"SBOL\" set " \
        "\"NUMBER\" = ?, \"SBOL_DATE\" = ?, " \
        "\"LCREDIT_ID\" = ?, " \
        "\"REMARK\" = ?, \"CCD_ID\" = ?, " \
        "\"UPDATE_USER_ID\" = ?, \"UPDATE_TIME\" = \'NOW\' " \
        "where \"SBOL_ID\" = ?";

      if (entityId())
      {        
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {                    
          query.bindValue(0, entityData_.sbolNumber);
          query.bindValue(1, entityData_.sbolDate);
          query.bindValue(2, entityData_.lcreditId ? entityData_.lcreditId.get() : QVariant(QVariant::LongLong));
          query.bindValue(3, entityData_.remark ? entityData_.remark.get() : QVariant(QVariant::String));
          query.bindValue(4, entityData_.ccdId);          
          query.bindValue(5, databaseModel()->userId());
          query.bindValue(6, entityData_.sbolId);
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

    bool SbolCardPage::insertIntoDatabase()
    {
      static const char* querySql = "insert into \"SBOL\" (" \
        "\"SBOL_ID\", \"NUMBER\", \"SBOL_DATE\", \"REMARK\", " \
        "\"CCD_ID\", \"LCREDIT_ID\", " \
        "\"CREATE_USER_ID\", \"UPDATE_USER_ID\", \"CREATE_TIME\", \"UPDATE_TIME\") " \
        "values(?, ?, ?, ?, ?, ?, ?, ?, \'NOW\', \'NOW\')";      

      if (OptionalQInt64 newEntityId = databaseModel()->generateId(sbolEntity))
      {
        entityData_.sbolId = newEntityId.get();    
        QSqlQuery query(databaseModel()->database());              
        if (query.prepare(querySql))
        {
          query.bindValue(0, entityData_.sbolId);
          query.bindValue(1, entityData_.sbolNumber);
          query.bindValue(2, databaseModel()->convertToServer(entityData_.sbolDate));          
          query.bindValue(3, entityData_.remark ? entityData_.remark.get() : QVariant(QVariant::String));
          query.bindValue(4, entityData_.ccdId);
          query.bindValue(5, entityData_.lcreditId ? entityData_.lcreditId.get() : QVariant(QVariant::LongLong));
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

    bool SbolCardPage::removeFromDatabase()
    {
      static const char* querySql = "delete from \"SBOL\" where \"SBOL_ID\" = ?";

      if (QMessageBox::Yes == QMessageBox::question(this, tr("Shipped bills of lading"), 
        tr("Delete shipped bill of lading?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
      {
        if (entityId())
        {
          QSqlQuery query(databaseModel()->database());              
          if (query.prepare(querySql))
          {
            query.bindValue(0, entityData_.sbolId);
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

    void SbolCardPage::on_lineEdit_textEdited(const QString&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void SbolCardPage::on_textEdit_textChanged()
    {
      contentsChanged_ = true;
      updateWidgets();
    }

    void SbolCardPage::on_dateEdit_dateChanged(const QDate&)
    {
      contentsChanged_ = true;
      updateWidgets();
    }    

    void SbolCardPage::on_ccdBtn_clicked(bool /*checked*/)
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
    
    void SbolCardPage::on_counterpartBtn_clicked(bool /*checked*/)
    {
      if (selectedCounterpartId_)
      {
        showCounterpartCardTaskWindow(selectedCounterpartId_.get());
      }
    }

    void SbolCardPage::on_lcreditBtn_clicked(bool /*checked*/)
    {
      if (viewMode == mode())
      {
        if (entityId() && entityData_.lcreditId)
        {          
          showLcreditCardTaskWindow(entityData_.lcreditId.get());
        }        
      }
      else
      {
        //todo: add default selection
        std::auto_ptr<ListPage> listPage(new LcreditListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), 
          OptionalInt(), UserDefinedFilter(), UserDefinedOrder(), 0));
        ListSelectDialog listSelectDialog(this, listPage);
        listSelectDialog.setWindowModality(Qt::WindowModal);
        if (QDialog::Accepted == listSelectDialog.exec())
        {          
          selectedLcreditId_ = listSelectDialog.listPage()->selectedId();          
          if (OptionalQString lcreditShortName = listSelectDialog.listPage()->selectedRelationalText())
          {
            ui_.lcreditEdit->setText(lcreditShortName.get());
          }
          contentsChanged_ = true;
          updateWidgets();
        }
      } 
    }    

    void SbolCardPage::updateCcdRelatedPersistData(qint64 ccdId)
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

    void SbolCardPage::updateCcdRelatedNonPersistData(qint64 ccdId)
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

    void SbolCardPage::on_removeLcreditBtn_clicked(bool /*checked*/)
    {
      if (viewMode != mode())
      {
        ui_.lcreditEdit->setText(QString());
        selectedLcreditId_.reset();        
        updateWidgets();
      }
    }

    void SbolCardPage::showRequiredMarkers(bool visible)
    {
      WidgetUtility::show(requiredMarkers_, visible);
    }

    void SbolCardPage::on_createUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.createUserId);
      }        
    }

    void SbolCardPage::on_updateUserBtn_clicked(bool /*checked*/)
    {
      if (entityId())
      {      
        showUserCardTaskWindow(entityData_.updateUserId);
      }        
    }

    void SbolCardPage::on_database_userUpdated(const ma::chrono::model::User& user)
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

    void SbolCardPage::on_database_ccdUpdated(const ma::chrono::model::Ccd& ccd)
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

    void SbolCardPage::on_database_ccdRemoved(const ma::chrono::model::Ccd& ccd)
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

    void SbolCardPage::on_database_lcreditUpdated(const ma::chrono::model::Lcredit& lcredit)
    {
      if (entityId() && entityData_.lcreditId && entityData_.lcreditId.get() == lcredit.lcreditId)
      {          
        lcreditNumber_ = lcredit.lcreditNumber;
      }      
      if (selectedLcreditId_ && selectedLcreditId_.get() == lcredit.lcreditId)
      {
        ui_.lcreditEdit->setText(lcredit.lcreditNumber);          
        updateWidgets();
      }      
    }

    void SbolCardPage::on_database_lcreditRemoved(const ma::chrono::model::Lcredit& lcredit)
    {
      if (selectedLcreditId_)
      {
        if (selectedLcreditId_.get() == lcredit.lcreditId)
        {
          ui_.lcreditEdit->setText(QString());
          selectedLcreditId_.reset();
          updateWidgets();
        }
      }
    }

    void SbolCardPage::on_database_counterpartUpdated(const ma::chrono::model::Counterpart& counterpart)
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

    void SbolCardPage::on_database_contractUpdated(const ma::chrono::model::Contract& contract)
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
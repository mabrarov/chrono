//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_BOLCARDPAGE_H
#define MA_CHRONO_BOLCARDPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ui_bolcardpage.h>
#include <ma/chrono/types.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/ccd_fwd.h>
#include <ma/chrono/model/counterpart_fwd.h>
#include <ma/chrono/model/contract_fwd.h>
#include <ma/chrono/model/autoenterprise_fwd.h>
#include <ma/chrono/model/bol.h>
#include <ma/chrono/cardpage.h>

namespace ma {
namespace chrono {

class BolCardPage : public CardPage
{
  Q_OBJECT

public: 
  BolCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent = 0);
  ~BolCardPage();

  void refresh();
  void save();
  void cancel();
  void edit();      
  void remove();
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;
            
private:
  Q_DISABLE_COPY(BolCardPage)

  void connectDataAwareWidgets();
  void updateWidgets();

  bool populateFromWidgets();
  bool populateFromDatabase();
  void populateWidgets();  
  void populateExportData();
  bool updateDatabase();
  bool insertIntoDatabase();
  bool removeFromDatabase();            
  void updateCcdRelatedPersistData(qint64 ccdId);
  void updateCcdRelatedNonPersistData(qint64 ccdId);
  void showRequiredMarkers(bool visible);

  Ui::bolCardPage ui_;
  QWidgetList dataAwareWidgets_;
  QWidgetList requiredMarkers_;
  bool        contentsChanged_;

  model::Bol     entityData_; 
  qint64         contractId_;
  OptionalQInt64 selectedContractId_;
  QString        ccdNumber_;
  OptionalQInt64 selectedCcdId_;
  QString        autoenterpriseShortName_;      
  OptionalQInt64 selectedAutoentrpriseId_;
  qint64         counterpartId_;
  QString        counterpartShortName_;
  OptionalQInt64 selectedCounterpartId_;
  QString        createUserName_;
  QString        updateUserName_;
      
private slots:
  void on_lineEdit_textEdited(const QString&);
  void on_textEdit_textChanged();
  void on_dateEdit_dateChanged(const QDate&);      
  void on_createUserBtn_clicked(bool checked = false);
  void on_updateUserBtn_clicked(bool checked = false);
  void on_ccdBtn_clicked(bool checked = false);      
  void on_counterpartBtn_clicked(bool checked = false);      
  void on_autoenterpriseBtn_clicked(bool checked = false);

  void on_database_userUpdated(const ma::chrono::model::User& user);      
  void on_database_ccdUpdated(const ma::chrono::model::Ccd& ccd);      
  void on_database_ccdRemoved(const ma::chrono::model::Ccd& ccd);      
  void on_database_autoenterpriseUpdated(
      const ma::chrono::model::Autoenterprise& autoenterprise);
  void on_database_autoenterpriseRemoved(
      const ma::chrono::model::Autoenterprise& autoenterprise);
  void on_database_counterpartUpdated(
      const ma::chrono::model::Counterpart& counterpart);
  void on_database_contractUpdated(
      const ma::chrono::model::Contract& contract);
}; // class BolCardPage    

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_BOLCARDPAGE_H

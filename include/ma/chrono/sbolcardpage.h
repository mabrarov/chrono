//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_SBOLCARDPAGE_H
#define MA_CHRONO_SBOLCARDPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ui_sbolcardpage.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/ccd_fwd.h>
#include <ma/chrono/model/counterpart_fwd.h>
#include <ma/chrono/model/contract_fwd.h>
#include <ma/chrono/model/lcredit_fwd.h>
#include <ma/chrono/model/sbol.h>
#include <ma/chrono/cardpage.h>

namespace ma {
namespace chrono {

class SbolCardPage : public CardPage
{
  Q_OBJECT

public: 
  SbolCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent = 0);
  ~SbolCardPage();

  void refresh();
  void save();
  void cancel();
  void edit();      
  void remove();
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;      
            
private:
  Q_DISABLE_COPY(SbolCardPage)

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

  Ui::sbolCardPage ui_;
  QWidgetList dataAwareWidgets_;    
  QWidgetList requiredMarkers_;
  bool contentsChanged_;      

  model::Sbol entityData_; 
  QString ccdNumber_;
  OptionalQString lcreditNumber_;
  qint64 contractId_;
  qint64 counterpartId_;
  QString counterpartShortName_;      
  QString createUserName_;
  QString updateUserName_;

  OptionalQInt64 selectedCcdId_;
  OptionalQInt64 selectedLcreditId_;
  OptionalQInt64 selectedContractId_;                  
  OptionalQInt64 selectedCounterpartId_;           
      
private slots:
  void on_lineEdit_textEdited(const QString&);
  void on_textEdit_textChanged();
  void on_dateEdit_dateChanged(const QDate&);      
  void on_createUserBtn_clicked(bool checked = false);
  void on_updateUserBtn_clicked(bool checked = false);
  void on_ccdBtn_clicked(bool checked = false);      
  void on_counterpartBtn_clicked(bool checked = false);      
  void on_lcreditBtn_clicked(bool checked = false);
  void on_removeLcreditBtn_clicked(bool checked = false);

  void on_database_userUpdated(const ma::chrono::model::User& user);
  void on_database_ccdUpdated(const ma::chrono::model::Ccd& ccd);      
  void on_database_ccdRemoved(const ma::chrono::model::Ccd& ccd);      
  void on_database_lcreditUpdated(const ma::chrono::model::Lcredit& lcredit);
  void on_database_lcreditRemoved(const ma::chrono::model::Lcredit& lcredit);
  void on_database_contractUpdated(
      const ma::chrono::model::Contract& contract);
  void on_database_counterpartUpdated(
      const ma::chrono::model::Counterpart& counterpart);
}; // class SbolCardPage    

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_SBOLCARDPAGE_H

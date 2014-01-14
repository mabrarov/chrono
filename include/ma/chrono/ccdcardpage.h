//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_CCDCARDPAGE_H
#define MA_CHRONO_CCDCARDPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ui_ccdcardpage.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/counterpart_fwd.h>
#include <ma/chrono/model/contract_fwd.h>
#include <ma/chrono/model/autoenterprise_fwd.h>
#include <ma/chrono/model/deppoint_fwd.h>
#include <ma/chrono/model/bank_fwd.h>
#include <ma/chrono/model/tterm_fwd.h>
#include <ma/chrono/model/currency_fwd.h>
#include <ma/chrono/model/ccd.h>
#include <ma/chrono/cardpage.h>

namespace ma {
namespace chrono {

class CcdCardPage : public CardPage
{
  Q_OBJECT

public: 
  CcdCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent = 0);
  ~CcdCardPage();

  void refresh();
  void save();
  void cancel();
  void edit();      
  void remove();
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;
      
  void setSelectedContractId(qint64 contractId);
            
private:
  Q_DISABLE_COPY(CcdCardPage)

  void connectDataAwareWidgets();
  void updateWidgets();

  bool populateFromWidgets();
  bool populateFromDatabase();
  void populateWidgets();  
  void populateExportData();
  bool updateDatabase();
  bool insertIntoDatabase();
  bool removeFromDatabase();                  
  void updateContractRelatedPersistData(qint64 contractId);
  void updateContractRelatedNonPersistData(qint64 contractId);
  void showRequiredMarkers(bool visible);

  Ui::ccdCardPage ui_;
  QWidgetList dataAwareWidgets_;    
  QWidgetList requiredMarkers_;
  bool        contentsChanged_;

  model::Ccd      entityData_;       
  QString         contractNumber_;
  OptionalQString contractPassportNumber_;
  qint64          counterpartId_;
  QString         counterpartShortName_;
  qint64          currencyId_;
  QString         currencyFullName_;
  QString         deppointShortName_;
  QString         ttermShortName_;
  qint64          bankId_;
  QString         bankShortName_;
  QString         createUserName_;
  QString         updateUserName_;
  OptionalQDate   paymentLimitDate_;
  QDate           zeroTaxLimitDate_;
            
  OptionalQInt64 selectedCounterpartId_;            
  OptionalQInt64 selectedCurrencyId_;
  OptionalQInt64 selectedContractId_;            
  OptionalQInt64 selectedDeppointId_;      
  OptionalQInt64 selectedTtermId_;            
  OptionalQInt64 selectedBankId_;      
            
private slots:
  void on_lineEdit_textEdited(const QString&);
  void on_textEdit_textChanged();
  void on_dateEdit_dateChanged(const QDate&);      
  void on_checkBox_stateChanged(int);
  void on_counterpartBtn_clicked(bool checked = false);
  void on_currencyBtn_clicked(bool checked = false);
  void on_contractBtn_clicked(bool checked = false);
  void on_deppointBtn_clicked(bool checked = false);      
  void on_bankBtn_clicked(bool checked = false);      
  void on_ttermBtn_clicked(bool checked = false);      
  void on_createUserBtn_clicked(bool checked = false);
  void on_updateUserBtn_clicked(bool checked = false);
  void on_exportCheck_stateChanged(int state);
  void on_paymentTermTypeCombo_currentIndexChanged(int index);      

  void on_database_userUpdated(const ma::chrono::model::User& user);      
  void on_database_counterpartUpdated(
      const ma::chrono::model::Counterpart& counterpart);
  void on_database_currencyUpdated(
      const ma::chrono::model::Currency& currency);
  void on_database_bankUpdated(const ma::chrono::model::Bank& bank);
  void on_database_contractUpdated(
      const ma::chrono::model::Contract& contract);
  void on_database_contractRemoved(
      const ma::chrono::model::Contract& contract);
  void on_database_deppointUpdated(
      const ma::chrono::model::Deppoint& deppoint);
  void on_database_deppointRemoved(
      const ma::chrono::model::Deppoint& deppoint);
  void on_database_ttermUpdated(const ma::chrono::model::Tterm& tterm);
  void on_database_ttermRemoved(const ma::chrono::model::Tterm& tterm);                              
}; // class CcdCardPage    

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_CCDCARDPAGE_H

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_CONTRACTCARDPAGE_H
#define MA_CHRONO_CONTRACTCARDPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ui_contractcardpage.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/counterpart_fwd.h>
#include <ma/chrono/model/bank_fwd.h>
#include <ma/chrono/model/resident_fwd.h>
#include <ma/chrono/model/currency_fwd.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/ccdlistpage.h>
#include <ma/chrono/contractccdlistpage_fwd.h>
#include <ma/chrono/contractinsurancelistpage_fwd.h>
#include <ma/chrono/contractpaysheetlistpage_fwd.h>
#include <ma/chrono/contractsbollistpage_fwd.h>
#include <ma/chrono/contractbollistpage_fwd.h>
#include <ma/chrono/cardpage.h>

namespace ma {
namespace chrono {

class ContractCardPage : public CardPage
{
  Q_OBJECT

public: 
  ContractCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent = 0);
  ~ContractCardPage();

  void refresh();
  void save();
  void cancel();
  void edit();      
  void remove();
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;
            
private:
  Q_DISABLE_COPY(ContractCardPage)

  void connectDataAwareWidgets();
  void updateWidgets();

  bool populateFromWidgets();
  bool populateFromDatabase();
  void populateWidgets();
  void populateExportData();      
  bool updateDatabase();
  bool insertIntoDatabase();
  bool removeFromDatabase();            
  void showRequiredMarkers(bool visible);

  Ui::contractCardPage ui_;
  ContractCcdListPage* contractCcdListPage_;
  ContractInsuranceListPage* contractInsuranceListPage_;
  ContractPaysheetListPage* contractPaysheetListPage_;
  ContractSbolListPage* contractSbolListPage_;
  ContractBolListPage* contractBolListPage_;
  QAction* relatedDocsAction_;

  QActionList ccdActions_;
  QActionList selectedCcdActions_;

  QActionList insuranceActions_;
  QActionList selectedInsuranceActions_;

  QActionList paysheetActions_;
  QActionList selectedPaysheetActions_;

  QActionList sbolActions_;
  QActionList selectedSbolActions_;

  QActionList bolActions_;
  QActionList selectedBolActions_;

  QWidgetList dataAwareWidgets_;    
  QWidgetList requiredMarkers_;
  bool contentsChanged_;

  model::Contract entityData_;  
  bool closeDataUpdated_;
  QString currencyFullName_;
  QString bankShortName_;
  QString counterpartShortName_;
  QString residentShortName_;
  QString createUserName_;      
  QString updateUserName_;
  OptionalQString closeUserName_;

  OptionalQInt64 selectedCurrencyId_;
  OptionalQInt64 selectedBankId_;
  OptionalQInt64 selectedCounterpartId_;
  OptionalQInt64 selectedResidentId_;      

private slots:
  void on_tabWidget_currentChanged(int index);      
  void on_ccd_doubleClicked(const QModelIndex& index);
  void on_insurance_doubleClicked(const QModelIndex& index);
  void on_paysheet_doubleClicked(const QModelIndex& index);
  void on_sbol_doubleClicked(const QModelIndex& index);
  void on_bol_doubleClicked(const QModelIndex& index);

  void on_lineEdit_textEdited(const QString&);
  void on_textEdit_textChanged();
  void on_dateEdit_dateChanged(const QDate&);
  void on_checkBox_stateChanged(int);
  void on_closedCheckBox_stateChanged(int);

  void on_currencyBtn_clicked(bool checked = false);
  void on_counterpartBtn_clicked(bool checked = false);
  void on_bankBtn_clicked(bool checked = false);
  void on_residentBtn_clicked(bool checked = false);
  void on_createUserBtn_clicked(bool checked = false);
  void on_updateUserBtn_clicked(bool checked = false);
  void on_closeUserBtn_clicked(bool checked = false);      

  void on_database_userUpdated(const ma::chrono::model::User& user);
  void on_database_currencyUpdated(
      const ma::chrono::model::Currency& currency);
  void on_database_currencyRemoved(
      const ma::chrono::model::Currency& currency);
  void on_database_counterpartUpdated(
      const ma::chrono::model::Counterpart& counterpart);
  void on_database_counterpartRemoved(
      const ma::chrono::model::Counterpart& counterpart);
  void on_database_bankUpdated(const ma::chrono::model::Bank& bank);
  void on_database_bankRemoved(const ma::chrono::model::Bank& bank);
  void on_database_residentUpdated(
      const ma::chrono::model::Resident& resident);
  void on_database_residentRemoved(
      const ma::chrono::model::Resident& resident);
}; // class ContractCardPage    

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_CONTRACTCARDPAGE_H

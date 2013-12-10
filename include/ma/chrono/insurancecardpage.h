//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_INSURANCECARDPAGE_H
#define MA_CHRONO_INSURANCECARDPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ui_insurancecardpage.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/contract_fwd.h>
#include <ma/chrono/model/currency_fwd.h>
#include <ma/chrono/model/insurance.h>
#include <ma/chrono/cardpage.h>

namespace ma {
namespace chrono {

class InsuranceCardPage : public CardPage
{
  Q_OBJECT

public: 
  InsuranceCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent = 0);
  ~InsuranceCardPage();

  void refresh();
  void save();
  void cancel();
  void edit();      
  void remove();
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;

  void setSelectedContractId(qint64 contractId);
            
private:
  Q_DISABLE_COPY(InsuranceCardPage)

  void connectDataAwareWidgets();
  void updateWidgets();

  bool populateFromWidgets();
  bool populateFromDatabase();
  void populateWidgets();     
  void populateExportData();
  bool updateDatabase();
  bool insertIntoDatabase();
  bool removeFromDatabase();            
  void updateContractRelatedNonPersistData(qint64 contractId);
  void showRequiredMarkers(bool visible);      

  Ui::insuranceCardPage ui_;
  QWidgetList dataAwareWidgets_;    
  QWidgetList requiredMarkers_;
  bool contentsChanged_;

  model::Insurance entityData_;
  QString contractNumber_;      
  QString currencyFullName_;
  QString createUserName_;      
  QString updateUserName_;

  OptionalQInt64 selectedContractId_;
  OptionalQInt64 selectedCurrencyId_;      

private slots:
  void on_lineEdit_textEdited(const QString&);
  void on_dateEdit_dateChanged(const QDate&);      
  void on_currencyBtn_clicked(bool checked = false);
  void on_contractBtn_clicked(bool checked = false);      
  void on_createUserBtn_clicked(bool checked = false);
  void on_updateUserBtn_clicked(bool checked = false);      

  void on_database_userUpdated(const ma::chrono::model::User& user);
  void on_database_currencyUpdated(
      const ma::chrono::model::Currency& currency);
  void on_database_currencyRemoved(
      const ma::chrono::model::Currency& currency);
  void on_database_contractUpdated(
      const ma::chrono::model::Contract& contract);
  void on_database_contractRemoved(
      const ma::chrono::model::Contract& contract);      
}; // class InsuranceCardPage    

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_INSURANCECARDPAGE_H

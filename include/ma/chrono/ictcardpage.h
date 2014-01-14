//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_ICTCARDPAGE_H
#define MA_CHRONO_ICTCARDPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/config.hpp>
#include <qglobal.h>
#include <ui_ictcardpage.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/currency_fwd.h>
#include <ma/chrono/model/contract_fwd.h>
#include <ma/chrono/model/ccd_fwd.h>
#include <ma/chrono/model/ict.h>
#include <ma/chrono/model/paysheet.h>
#include <ma/chrono/model/bank_fwd.h>
#include <ma/chrono/cardpage.h>

QT_BEGIN_NAMESPACE
class QSqlQuery;  
QT_END_NAMESPACE

namespace ma {
namespace chrono {

class IctCardPage : public CardPage
{
  Q_OBJECT

public: 
  IctCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent = 0);      
  ~IctCardPage();

  void refresh();
  void save();
  void cancel();
  void edit();      
  void remove();
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;
            
private:
  BOOST_STATIC_CONSTANT(int, paysheetDataRole = Qt::UserRole);
  
  struct PaysheetData
  {
    model::Paysheet          paysheet;        
    QString                  currencyAlphabeticCode;
    QString                  contractNumber;        
    OptionalQString passportNumber;
    OptionalQString ccdNumber;
  }; // struct PaysheetData

  typedef QVector<PaysheetData> PaysheetDataVector;
      

  Q_DISABLE_COPY(IctCardPage)

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
  void updatePaysheetTableWidgets();
  bool addPaysheet(qint64 paysheetId);

  PaysheetDataVector readPaysheetData(QSqlQuery& query) const;
  bool insertPaysheetData(QSqlQuery& query, qint64 ictId, 
      const PaysheetDataVector& paysheetData) const;
  static bool contains(const PaysheetDataVector& paysheetData, 
      qint64 paysheetId);
  static bool contains(const PaysheetDataVector& paysheetData, 
      const ma::chrono::model::Paysheet& paysheet);
  static bool contains(const PaysheetDataVector& paysheetData, 
      const ma::chrono::model::Currency& currency);
  static bool contains(const PaysheetDataVector& paysheetData, 
      const ma::chrono::model::Contract& contract);
  static bool contains(const PaysheetDataVector& paysheetData, 
      const ma::chrono::model::Ccd& ccd);
  static void update(PaysheetDataVector& paysheetData, 
      const PaysheetData& updatedPaysheetData);
  static void update(PaysheetDataVector& paysheetData, 
      const ma::chrono::model::Currency& currency);
  static void update(PaysheetDataVector& paysheetData, 
      const ma::chrono::model::Contract& contract);
  static void update(PaysheetDataVector& paysheetData, 
      const ma::chrono::model::Ccd& ccd);
  static void remove(PaysheetDataVector& paysheetData, qint64 paysheetId);
  boost::optional<PaysheetData> readPaysheetData(qint64 paysheetId) const;            
  void populatePaysheetTable(const PaysheetDataVector& vector);

  Ui::ictCardPage ui_;
  QWidgetList dataAwareWidgets_;    
  QWidgetList requiredMarkers_;
  bool contentsChanged_;

  model::Ict entityData_;  
  int bankCode_;
  QString bankShortName_;            
  QString createUserName_;
  QString updateUserName_;      
  OptionalQInt64 selectedBankId_;
  PaysheetDataVector paysheetTableData_;
  PaysheetDataVector nonPersistPaysheetTableData_;
      
private slots:
  void on_lineEdit_textEdited(const QString&);
  void on_textEdit_textChanged();
  void on_dateEdit_dateChanged(const QDate&);      
  void on_createUserBtn_clicked(bool checked = false);
  void on_updateUserBtn_clicked(bool checked = false);
  void on_bankBtn_clicked(bool checked = false);      
  void on_addPaysheetBtn_clicked(bool checked = false);      
  void on_removePaysheetBtn_clicked(bool checked = false);      
  void on_moveUpBtn_clicked(bool checked = false);
  void on_moveDownBtn_clicked(bool checked = false);
  void on_paysheetTable_currentCellChanged(int currentRow, int currentColumn,
      int previousRow, int previousColumn);
  void on_paysheetTable_cellDoubleClicked(int row, int column);
  void on_paysheetTable_headerContextMenu_requested(const QPoint& pos);
  void on_stateCheck_stateChanged(int state);      

  void on_database_userUpdated(const ma::chrono::model::User& user);            
  void on_database_bankUpdated(const ma::chrono::model::Bank& bank);
  void on_database_bankRemoved(const ma::chrono::model::Bank& bank);      
  void on_database_paysheetUpdated(
      const ma::chrono::model::Paysheet& paysheet);
  void on_database_paysheetRemoved(
      const ma::chrono::model::Paysheet& paysheet);
  void on_database_currencyUpdated(
      const ma::chrono::model::Currency& currency);
  void on_database_contractUpdated(
      const ma::chrono::model::Contract& contract);
  void on_database_ccdUpdated(const ma::chrono::model::Ccd& ccd);

  virtual void on_sendToExcelAction_triggered();

  //void on_database_counterpartUpdated(
  //    const ma::chrono::model::Counterpart& counterpart);
}; // class IctCardPage    

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_ICTCARDPAGE_H

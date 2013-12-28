//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_ICDOCCARDPAGE_H
#define MA_CHRONO_ICDOCCARDPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ui_icdoccardpage.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/currency_fwd.h>
#include <ma/chrono/model/contract_fwd.h>
#include <ma/chrono/model/icdoc.h>
#include <ma/chrono/model/ccd.h>
#include <ma/chrono/model/bank_fwd.h>
#include <ma/chrono/cardpage.h>

QT_BEGIN_NAMESPACE
class QSqlQuery;  
QT_END_NAMESPACE

namespace ma {
namespace chrono {

class IcdocCardPage : public CardPage
{
  Q_OBJECT

public: 
  IcdocCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent = 0);      
  ~IcdocCardPage();

  void refresh();
  void save();
  void cancel();
  void edit();      
  void remove();
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;
            
private:
  static const int ccdDataRole = Qt::UserRole;

  struct CcdData
  {
    model::Ccd               ccd;
    OptionalQDate   paymentLimitDate;
    QDate                    zeroTaxLimitDate;
    QString                  contractNumber;        
    OptionalQString passportNumber;
    qint64                   currencyId;
    QString                  currencyAlphabeticCode;
  }; // struct CcdData

  typedef QVector<CcdData> CcdDataVector;
      
  Q_DISABLE_COPY(IcdocCardPage)

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
  void updateCcdTableWidgets();
  bool addCcd(qint64 ccdId);

  CcdDataVector readCcdData(QSqlQuery& query) const;
  bool insertCcdData(QSqlQuery& query, qint64 icdocId, 
      const CcdDataVector& ccdData) const;
  static bool contains(const CcdDataVector& ccdData, qint64 ccdId);
  static bool contains(const CcdDataVector& ccdData, 
      const ma::chrono::model::Ccd& ccd);
  static bool contains(const CcdDataVector& ccdData, 
      const ma::chrono::model::Currency& currency);
  static bool contains(const CcdDataVector& ccdData, 
      const ma::chrono::model::Contract& contract);
  static void update(CcdDataVector& ccdData, const CcdData& updatedCcdData);
  static void update(CcdDataVector& ccdData, 
      const ma::chrono::model::Currency& currency);
  static void update(CcdDataVector& ccdData, 
      const ma::chrono::model::Contract& contract);
  static void remove(CcdDataVector& ccdData, qint64 ccdId);
  boost::optional<CcdData> readCcdData(qint64 ccdId) const;            
  void populateCcdTable(const CcdDataVector& vector);

  Ui::icdocCardPage ui_;
  QWidgetList dataAwareWidgets_;    
  QWidgetList requiredMarkers_;
  bool contentsChanged_;

  model::Icdoc entityData_;   
  int bankCode_;
  QString bankShortName_;            
  QString createUserName_;
  QString updateUserName_;      
  OptionalQInt64 selectedBankId_;
  CcdDataVector ccdTableData_;
  CcdDataVector nonPersistCcdTableData_;
      
private slots:
  void on_lineEdit_textEdited(const QString&);
  void on_textEdit_textChanged();
  void on_dateEdit_dateChanged(const QDate&);      
  void on_createUserBtn_clicked(bool checked = false);
  void on_updateUserBtn_clicked(bool checked = false);
  void on_bankBtn_clicked(bool checked = false);      
  void on_addCcdBtn_clicked(bool checked = false);      
  void on_removeCcdBtn_clicked(bool checked = false);      
  void on_moveUpBtn_clicked(bool checked = false);
  void on_moveDownBtn_clicked(bool checked = false);
  void on_ccdTable_currentCellChanged(int currentRow, int currentColumn,
      int previousRow, int previousColumn);
  void on_ccdTable_cellDoubleClicked(int row, int column);
  void on_ccdTable_headerContextMenu_requested(const QPoint& pos);
  void on_stateCheck_stateChanged(int state);      

  void on_database_userUpdated(const ma::chrono::model::User& user);            
  void on_database_bankUpdated(const ma::chrono::model::Bank& bank);
  void on_database_bankRemoved(const ma::chrono::model::Bank& bank);      
  void on_database_ccdUpdated(const ma::chrono::model::Ccd& ccd);
  void on_database_ccdRemoved(const ma::chrono::model::Ccd& ccd);
  void on_database_currencyUpdated(
      const ma::chrono::model::Currency& currency);
  void on_database_contractUpdated(
      const ma::chrono::model::Contract& contract);

  virtual void on_sendToExcelAction_triggered();  
}; // class IcdocCardPage    

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_ICDOCCARDPAGE_H

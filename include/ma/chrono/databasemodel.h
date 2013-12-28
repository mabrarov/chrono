//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_DATABASEMODEL_H
#define MA_CHRONO_DATABASEMODEL_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <QObject>
#include <QDateTime>
#include <QMap>
#include <QSqlDatabase>
#include <ma/chrono/types.h>
#include <ma/chrono/dbconstants.h>
#include <ma/chrono/generator_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/country_fwd.h>
#include <ma/chrono/model/currency_fwd.h>
#include <ma/chrono/model/tterm_fwd.h>
#include <ma/chrono/model/bank_fwd.h>
#include <ma/chrono/model/autoenterprise_fwd.h>
#include <ma/chrono/model/counterpart_fwd.h>
#include <ma/chrono/model/resident_fwd.h>
#include <ma/chrono/model/deppoint_fwd.h>
#include <ma/chrono/model/contract_fwd.h>
#include <ma/chrono/model/insurance_fwd.h>
#include <ma/chrono/model/ccd_fwd.h>
#include <ma/chrono/model/bol_fwd.h>
#include <ma/chrono/model/sbol_fwd.h>
#include <ma/chrono/model/lcredit_fwd.h>
#include <ma/chrono/model/ict_fwd.h>
#include <ma/chrono/model/icdoc_fwd.h>
#include <ma/chrono/model/paysheet_fwd.h>
#include <ma/chrono/databasemodel_fwd.h>

QT_BEGIN_NAMESPACE
class QAbstractTableModel;
QT_END_NAMESPACE

namespace ma {
namespace chrono {

class DatabaseModel : public QObject
{
  Q_OBJECT

public:
  enum OpenError
  {
    openSuccess = 0,
    databaseClientError,
    databaseConnectError,
    invalidUserCredentials,
    unknownError
  };      

  DatabaseModel();
  ~DatabaseModel();      
  void open(const QString& host, const QString& databaseName,
      const QString& databaseUser, const QString& databasePassword,
      const QString& clientUser, const QString& clientPassword,
      OpenError& openError);   
  bool isOpen() const;        
  qint64 userId() const;      
  QString userName() const;
  QSqlDatabase database() const;
  OptionalQInt64 generateId(Entity entity) const;            

  QDateTime convertFromServer(const QDateTime& dateTime) const;      
  QDate     convertFromServer(const QDate& date) const;
  QDateTime convertToServer(const QDateTime& dateTime) const;
  QDate     convertToServer(const QDate& date) const;

  void notifyUpdate(const model::User& user);
  void notifyInsert(const model::User& user);
  void notifyRemove(const model::User& user);      

  void notifyUpdate(const model::Country& country);
  void notifyInsert(const model::Country& country);
  void notifyRemove(const model::Country& country);      

  void notifyUpdate(const model::Currency& currency);
  void notifyInsert(const model::Currency& currency);
  void notifyRemove(const model::Currency& currency);      

  void notifyUpdate(const model::Tterm& tterm);
  void notifyInsert(const model::Tterm& tterm);
  void notifyRemove(const model::Tterm& tterm); 

  void notifyUpdate(const model::Bank& bank);
  void notifyInsert(const model::Bank& bank);
  void notifyRemove(const model::Bank& bank); 

  void notifyUpdate(const model::Autoenterprise& autoenterprise);
  void notifyInsert(const model::Autoenterprise& autoenterprise);
  void notifyRemove(const model::Autoenterprise& autoenterprise); 

  void notifyUpdate(const model::Counterpart& counterpart);
  void notifyInsert(const model::Counterpart& counterpart);
  void notifyRemove(const model::Counterpart& counterpart);

  void notifyUpdate(const model::Resident& resident);
  void notifyInsert(const model::Resident& resident);
  void notifyRemove(const model::Resident& resident);

  void notifyUpdate(const model::Deppoint& deppoint);
  void notifyInsert(const model::Deppoint& deppoint);
  void notifyRemove(const model::Deppoint& deppoint);

  void notifyUpdate(const model::Contract& contract);
  void notifyInsert(const model::Contract& contract);
  void notifyRemove(const model::Contract& contract);

  void notifyUpdate(const model::Insurance& insurance);
  void notifyInsert(const model::Insurance& insurance);
  void notifyRemove(const model::Insurance& insurance);

  void notifyUpdate(const model::Ccd& ccd);
  void notifyInsert(const model::Ccd& ccd);
  void notifyRemove(const model::Ccd& ccd);

  void notifyUpdate(const model::Bol& bol);
  void notifyInsert(const model::Bol& bol);
  void notifyRemove(const model::Bol& bol);

  void notifyUpdate(const model::Sbol& sbol);
  void notifyInsert(const model::Sbol& sbol);
  void notifyRemove(const model::Sbol& sbol);

  void notifyUpdate(const model::Lcredit& lcredit);
  void notifyInsert(const model::Lcredit& lcredit);
  void notifyRemove(const model::Lcredit& lcredit);      

  void notifyUpdate(const model::Ict& ict);
  void notifyInsert(const model::Ict& ict);
  void notifyRemove(const model::Ict& ict);

  void notifyUpdate(const model::Icdoc& icdoc);
  void notifyInsert(const model::Icdoc& icdoc);
  void notifyRemove(const model::Icdoc& icdoc);

  void notifyUpdate(const model::Paysheet& paysheet);
  void notifyInsert(const model::Paysheet& paysheet);
  void notifyRemove(const model::Paysheet& paysheet);

public slots:                  
  void close();

signals:
  void connectionStateChanged();
  void aboutToClose();

  void userUpdated(const ma::chrono::model::User& user);
  void userInserted(const ma::chrono::model::User& user);
  void userRemoved(const ma::chrono::model::User& user);

  void countryUpdated(const ma::chrono::model::Country& country);
  void countryInserted(const ma::chrono::model::Country& country);
  void countryRemoved(const ma::chrono::model::Country& country);

  void currencyUpdated(const ma::chrono::model::Currency& currency);
  void currencyInserted(const ma::chrono::model::Currency& currency);
  void currencyRemoved(const ma::chrono::model::Currency& currency);

  void ttermUpdated(const ma::chrono::model::Tterm& tterm);
  void ttermInserted(const ma::chrono::model::Tterm& tterm);
  void ttermRemoved(const ma::chrono::model::Tterm& tterm);

  void bankUpdated(const ma::chrono::model::Bank& bank);
  void bankInserted(const ma::chrono::model::Bank& bank);
  void bankRemoved(const ma::chrono::model::Bank& bank);

  void autoenterpriseUpdated(
      const ma::chrono::model::Autoenterprise& autoenterprise);
  void autoenterpriseInserted(
      const ma::chrono::model::Autoenterprise& autoenterprise);
  void autoenterpriseRemoved(
      const ma::chrono::model::Autoenterprise& autoenterprise);

  void counterpartUpdated(const ma::chrono::model::Counterpart& counterpart);
  void counterpartInserted(const ma::chrono::model::Counterpart& counterpart);
  void counterpartRemoved(const ma::chrono::model::Counterpart& counterpart);

  void residentUpdated(const ma::chrono::model::Resident& resident);
  void residentInserted(const ma::chrono::model::Resident& resident);
  void residentRemoved(const ma::chrono::model::Resident& resident);

  void deppointUpdated(const ma::chrono::model::Deppoint& deppoint);
  void deppointInserted(const ma::chrono::model::Deppoint& deppoint);
  void deppointRemoved(const ma::chrono::model::Deppoint& deppoint);

  void contractUpdated(const ma::chrono::model::Contract& contract);
  void contractInserted(const ma::chrono::model::Contract& contract);
  void contractRemoved(const ma::chrono::model::Contract& contract);

  void insuranceUpdated(const ma::chrono::model::Insurance& insurance);
  void insuranceInserted(const ma::chrono::model::Insurance& insurance);
  void insuranceRemoved(const ma::chrono::model::Insurance& insurance);

  void ccdUpdated(const ma::chrono::model::Ccd& ccd);
  void ccdInserted(const ma::chrono::model::Ccd& ccd);
  void ccdRemoved(const ma::chrono::model::Ccd& ccd);

  void bolUpdated(const ma::chrono::model::Bol& bol);
  void bolInserted(const ma::chrono::model::Bol& bol);
  void bolRemoved(const ma::chrono::model::Bol& bol);

  void sbolUpdated(const ma::chrono::model::Sbol& sbol);
  void sbolInserted(const ma::chrono::model::Sbol& sbol);
  void sbolRemoved(const ma::chrono::model::Sbol& sbol);

  void lcreditUpdated(const ma::chrono::model::Lcredit& lcredit);
  void lcreditInserted(const ma::chrono::model::Lcredit& lcredit);
  void lcreditRemoved(const ma::chrono::model::Lcredit& lcredit);            

  void ictUpdated(const ma::chrono::model::Ict& ict);
  void ictInserted(const ma::chrono::model::Ict& ict);
  void ictRemoved(const ma::chrono::model::Ict& ict);

  void icdocUpdated(const ma::chrono::model::Icdoc& icdoc);
  void icdocInserted(const ma::chrono::model::Icdoc& icdoc);
  void icdocRemoved(const ma::chrono::model::Icdoc& icdoc);

  void paysheetUpdated(const ma::chrono::model::Paysheet& paysheet);
  void paysheetInserted(const ma::chrono::model::Paysheet& paysheet);
  void paysheetRemoved(const ma::chrono::model::Paysheet& paysheet);

private:
  Q_DISABLE_COPY(DatabaseModel)
                  
  qint64 userId_;
  QString userName_;
  QSqlDatabase database_;
  QMap<Entity, boost::shared_ptr<Generator> > generators_;
}; // class DatabaseModel

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_DATABASEMODEL_H

/*
TRANSLATOR ma::chrono::DatabaseModel
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <boost/make_shared.hpp>
#include <QVariant>
#include <QApplication>
#include <QSqlDriver>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <ma/chrono/generator.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/model/country.h>
#include <ma/chrono/model/currency.h>
#include <ma/chrono/model/tterm.h>
#include <ma/chrono/model/bank.h>
#include <ma/chrono/model/autoenterprise.h>
#include <ma/chrono/model/deppoint.h>
#include <ma/chrono/model/counterpart.h>
#include <ma/chrono/model/resident.h>
#include <ma/chrono/model/contract.h>
#include <ma/chrono/model/ccd.h>
#include <ma/chrono/model/bol.h>
#include <ma/chrono/model/currencyamount.h>
#include <ma/chrono/model/rateamount.h>
#include <ma/chrono/model/insurance.h>
#include <ma/chrono/model/ict.h>
#include <ma/chrono/model/icdoc.h>
#include <ma/chrono/model/paysheet.h>
#include <ma/chrono/databasemodel.h>

namespace ma {

namespace chrono {  

int currencyAmountTypeId;
int rateAmountTypeId;

void registerDatabaseMetaTypes()
{
  rateAmountTypeId = 
      qRegisterMetaType<model::RateAmount>("ma::chrono::model::RateAmount");
  qRegisterMetaType<model::User>("ma::chrono::model::User");
  qRegisterMetaType<model::Country>("ma::chrono::model::Country");
  qRegisterMetaType<model::Currency>("ma::chrono::model::Currency");
  qRegisterMetaType<model::Tterm>("ma::chrono::model::Tterm");
  qRegisterMetaType<model::Bank>("ma::chrono::model::Bank");
  qRegisterMetaType<model::Autoenterprise>("ma::chrono::model::Autoenterprise");
  qRegisterMetaType<model::Counterpart>("ma::chrono::model::Counterpart");
  qRegisterMetaType<model::Deppoint>("ma::chrono::model::Deppoint");
  qRegisterMetaType<model::Contract>("ma::chrono::model::Contract");
  qRegisterMetaType<model::Ccd>("ma::chrono::model::Ccd");            
  currencyAmountTypeId = 
      qRegisterMetaType<model::CurrencyAmount>("ma::chrono::model::CurrencyAmount");
  qRegisterMetaType<model::Insurance>("ma::chrono::model::Insurance");
  qRegisterMetaType<model::Bol>("ma::chrono::model::Bol");      
  qRegisterMetaType<model::Ict>("ma::chrono::model::Ict");
  qRegisterMetaType<model::Icdoc>("ma::chrono::model::Icdoc");
  qRegisterMetaType<model::Paysheet>("ma::chrono::model::Paysheet");
}

DatabaseModel::DatabaseModel()
  : QObject()            
  , userId_(0)
  , database_(QSqlDatabase::addDatabase("QIBASE"))
  , generators_()      
{
  Generator::QueryOption queryOption(
      "\"GENERATOR\"", "\"GENERATOR_ID\"", "\"LAST_VALUE\"");
  Generator::ConcurrentOption concurrentOption(
      4, 5, boost::posix_time::milliseconds(300));
  for (int i = userEntity; i != entityCount; ++i)
  {
    generators_.insert(static_cast<Entity>(i), 
        boost::make_shared<Generator>(queryOption, concurrentOption, i));
  }      
}

DatabaseModel::~DatabaseModel()
{
}

void DatabaseModel::open(const QString& host, const QString& databaseName,
    const QString& databaseUser, const QString& databasePassword,
    const QString& clientUser, const QString& clientPassword,
    OpenError& openError)
{
  if (!database_.isValid())
  {
    openError = databaseClientError;
    return;
  }
  database_.close();
  database_.setHostName(host);
  database_.setDatabaseName(databaseName);
  database_.setUserName(databaseUser);
  database_.setPassword(databasePassword);
  if (!database_.open())
  {        
    openError = databaseConnectError;
    return;        
  }
  QSqlQuery userQuery(database_); 
  if (!userQuery.prepare(
      "select \"USER_ID\" from \"USER\" where \"LOGIN\" = ? and \"PASS_HASH\" = ?"))
  {
    openError = databaseClientError;
    database_.close();
    return;
  }
  userQuery.bindValue(0, clientUser);
  userQuery.bindValue(1, clientPassword);
  if (!userQuery.exec())
  {
    openError = databaseClientError;
    database_.close();
    return;
  }
  if (!userQuery.next())
  {
    openError = invalidUserCredentials;
    database_.close();
    return;        
  }
  userId_ = userQuery.value(0).toLongLong();
  userName_ = clientUser;
  openError = openSuccess;
  emit connectionStateChanged();
}    

QDateTime DatabaseModel::convertFromServer(const QDateTime& dateTime) const
{
  return dateTime;
}

QDate DatabaseModel::convertFromServer(const QDate& date) const
{
  return date;
}

QDateTime DatabaseModel::convertToServer(const QDateTime& dateTime) const
{
  return dateTime;
}

QDate DatabaseModel::convertToServer(const QDate& date) const
{
  return date;
}

void DatabaseModel::notifyUpdate(const model::User& user)
{
  if (user.userId == userId_)
  {
    userName_ = user.userName;
  }      
  emit userUpdated(user);
}

void DatabaseModel::notifyInsert(const model::User& user)
{
  emit userInserted(user);
}

void DatabaseModel::notifyRemove(const model::User& user)
{
  emit userRemoved(user);
}

void DatabaseModel::notifyUpdate(const model::Country& country)
{      
  emit countryUpdated(country);
}

void DatabaseModel::notifyInsert(const model::Country& country)
{      
  emit countryInserted(country);
}

void DatabaseModel::notifyRemove(const model::Country& country)
{      
  emit countryRemoved(country);
}

void DatabaseModel::notifyUpdate(const model::Currency& currency)
{      
  emit currencyUpdated(currency);
}

void DatabaseModel::notifyInsert(const model::Currency& currency)
{      
  emit currencyInserted(currency);
}

void DatabaseModel::notifyRemove(const model::Currency& currency)
{
  emit currencyRemoved(currency);
}

void DatabaseModel::notifyUpdate(const model::Tterm& tterm)
{      
  emit ttermUpdated(tterm);
}

void DatabaseModel::notifyInsert(const model::Tterm& tterm)
{      
  emit ttermInserted(tterm);
}

void DatabaseModel::notifyRemove(const model::Tterm& tterm)
{      
  emit ttermRemoved(tterm);
}

void DatabaseModel::notifyUpdate(const model::Bank& bank)
{      
  emit bankUpdated(bank);
}

void DatabaseModel::notifyInsert(const model::Bank& bank)
{
  emit bankInserted(bank);
}

void DatabaseModel::notifyRemove(const model::Bank& bank)
{
  emit bankRemoved(bank);
}

void DatabaseModel::notifyUpdate(const model::Autoenterprise& autoenterprise)
{      
  emit autoenterpriseUpdated(autoenterprise);
}

void DatabaseModel::notifyInsert(const model::Autoenterprise& autoenterprise)
{
  emit autoenterpriseInserted(autoenterprise);
}

void DatabaseModel::notifyRemove(const model::Autoenterprise& autoenterprise)
{
  emit autoenterpriseRemoved(autoenterprise);
}

void DatabaseModel::notifyUpdate(const model::Counterpart& counterpart)
{ 
  emit counterpartUpdated(counterpart);
}

void DatabaseModel::notifyInsert(const model::Counterpart& counterpart)
{
  emit counterpartInserted(counterpart);
}

void DatabaseModel::notifyRemove(const model::Counterpart& counterpart)
{
  emit counterpartRemoved(counterpart);
}

void DatabaseModel::notifyUpdate(const model::Resident& resident)
{      
  emit residentUpdated(resident);
}

void DatabaseModel::notifyInsert(const model::Resident& resident)
{
  emit residentInserted(resident);
}

void DatabaseModel::notifyRemove(const model::Resident& resident)
{      
  emit residentRemoved(resident);
}

void DatabaseModel::notifyUpdate(const model::Deppoint& deppoint)
{      
  emit deppointUpdated(deppoint);
}

void DatabaseModel::notifyInsert(const model::Deppoint& deppoint)
{            
  emit deppointInserted(deppoint);
}

void DatabaseModel::notifyRemove(const model::Deppoint& deppoint)
{      
  emit deppointRemoved(deppoint);
}

void DatabaseModel::notifyUpdate(const model::Contract& contract)
{
  emit contractUpdated(contract);
}

void DatabaseModel::notifyInsert(const model::Contract& contract)
{
  emit contractInserted(contract);
}

void DatabaseModel::notifyRemove(const model::Contract& contract)
{
  emit contractRemoved(contract);
}

void DatabaseModel::notifyUpdate(const model::Insurance& insurance)
{
  emit insuranceUpdated(insurance);
}

void DatabaseModel::notifyInsert(const model::Insurance& insurance)
{
  emit insuranceInserted(insurance);
}

void DatabaseModel::notifyRemove(const model::Insurance& insurance)
{
  emit insuranceRemoved(insurance);
}

void DatabaseModel::notifyUpdate(const model::Ccd& ccd)
{      
  emit ccdUpdated(ccd);
}

void DatabaseModel::notifyInsert(const model::Ccd& ccd)
{     
  emit ccdInserted(ccd);
}

void DatabaseModel::notifyRemove(const model::Ccd& ccd)
{     
  emit ccdRemoved(ccd);
}

void DatabaseModel::notifyUpdate(const model::Bol& bol)
{      
  emit bolUpdated(bol);
}

void DatabaseModel::notifyInsert(const model::Bol& bol)
{     
  emit bolInserted(bol);
}

void DatabaseModel::notifyRemove(const model::Bol& bol)
{     
  emit bolRemoved(bol);
}

void DatabaseModel::notifyUpdate(const model::Sbol& sbol)
{      
  emit sbolUpdated(sbol);
}

void DatabaseModel::notifyInsert(const model::Sbol& sbol)
{     
  emit sbolInserted(sbol);
}

void DatabaseModel::notifyRemove(const model::Sbol& sbol)
{     
  emit sbolRemoved(sbol);
}

void DatabaseModel::notifyUpdate(const model::Lcredit& lcredit)
{      
  emit lcreditUpdated(lcredit);
}

void DatabaseModel::notifyInsert(const model::Lcredit& lcredit)
{     
  emit lcreditInserted(lcredit);
}

void DatabaseModel::notifyRemove(const model::Lcredit& lcredit)
{     
  emit lcreditRemoved(lcredit);
}    

void DatabaseModel::notifyUpdate(const model::Ict& ict)
{
  emit ictUpdated(ict);
}

void DatabaseModel::notifyInsert(const model::Ict& ict)
{
  emit ictInserted(ict);
}

void DatabaseModel::notifyRemove(const model::Ict& ict)
{
  emit ictRemoved(ict);
}

void DatabaseModel::notifyUpdate(const model::Icdoc& icdoc)
{
  emit icdocUpdated(icdoc);
}

void DatabaseModel::notifyInsert(const model::Icdoc& icdoc)
{
  emit icdocInserted(icdoc);
}

void DatabaseModel::notifyRemove(const model::Icdoc& icdoc)
{
  emit icdocRemoved(icdoc);
}

void DatabaseModel::notifyUpdate(const model::Paysheet& paysheet)
{      
  emit paysheetUpdated(paysheet);
}

void DatabaseModel::notifyInsert(const model::Paysheet& paysheet)
{     
  emit paysheetInserted(paysheet);
}

void DatabaseModel::notifyRemove(const model::Paysheet& paysheet)
{     
  emit paysheetRemoved(paysheet);
}

void DatabaseModel::close()
{
  if (database_.isOpen())
  {
    emit aboutToClose();        
    database_.close();
    emit connectionStateChanged();        
  }
}

bool DatabaseModel::isOpen() const
{
  return database_.isOpen();
}

qint64 DatabaseModel::userId() const
{
  return userId_;
}

QString DatabaseModel::userName() const
{
  return userName_;
}

QSqlDatabase DatabaseModel::database() const
{
  return database_;
}

OptionalQInt64 DatabaseModel::generateId(Entity entity) const
{
  OptionalQInt64 id;
  boost::shared_ptr<Generator> generator = generators_.value(entity);
  if (generator)
  {
    try 
    {
      id.reset(generator->generateNext(database_));
    }
    catch (...)
    {
      //todo add logging
    }
  }
  return id;
}

} // namespace chrono
} //namespace ma

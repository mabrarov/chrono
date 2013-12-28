/*
TRANSLATOR ma::chrono::ContractInsuranceTableModel
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <boost/make_shared.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <ma/chrono/dateutility.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/model/currencyamount.h>
#include <ma/chrono/contractinsurancetablemodel.h>

namespace ma {

namespace chrono {

namespace {    

const int queryColumnCount = 11;
const char* selectPartSql = 
  "select t.\"INSURANCE_ID\", t.\"POLICY_NUMBER\", t.\"AMOUNT\", cur.\"ALPHABETIC_CODE\", " \
  "t.\"SIGNING_DATE\", t.\"START_DATE\", t.\"END_DATE\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
  "from \"INSURANCE\" t " \
  "join \"CURRENCY\" cur on t.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\"";

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "Policy number"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "Amount"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "Currency"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "Signing date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "Start date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "End date"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "User, last update"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "Time, last update")        
};            

const int selectedFieldCount = 8;

const char* selectedFieldNames[selectedFieldCount] = 
{
  "t.\"POLICY_NUMBER\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"SIGNING_DATE\"",
  "t.\"START_DATE\""
  "t.\"END_DATE\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "Policy number"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "Signing date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "Start date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "End date"),              
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractInsuranceTableModel", "Time, last update")
};

const bool selectedFieldNullability[selectedFieldCount] = 
{
  false,        
  false,
  false,
  false,
  false,
  false,
  false,
  false        
};

const FieldDescription::FieldType selectedFieldTypes[selectedFieldCount] = 
{      
  FieldDescription::stringField,        
  FieldDescription::entityField,
  FieldDescription::entityField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField
};

const Entity selectedFieldEntitySubTypes[selectedFieldCount] = 
{
  unknownEntity,
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity
};

} // namespace

ContractInsuranceTableModel::ContractInsuranceTableModel(
    const DatabaseModelPtr& databaseModel, 
    QObject* parent, 
    const OptionalQInt64& contractId, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)
  , contractId_(contractId)
{
}

ContractInsuranceTableModel::~ContractInsuranceTableModel()
{
}    

int ContractInsuranceTableModel::columnCount(
    const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant ContractInsuranceTableModel::headerData(int section, 
    Qt::Orientation orientation, int role) const
{       
  if (Qt::Horizontal == orientation)
  {      
    if (Qt::DisplayRole == role)
    {
      if (queryColumnCount > section)
      {
        return tr(columnHeaders[section]);
      }          
    }
    else if (Qt::TextAlignmentRole == role)
    {
      if (1 == section || 2 == section)
      {
        return QVariant(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
      }          
      else if (section)
      {
        return QVariant(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
      }
    }
  }
  return QSqlQueryModel::headerData(section, orientation, role);
}

QVariant ContractInsuranceTableModel::data(
    const QModelIndex& index, int role) const
{
  int column = index.column();
  if (Qt::DisplayRole == role)
  {        
    if (9 == column || 10 == column)
    {        
      QVariant var = QSqlQueryModel::data(index, role);
      if (var.canConvert<QDateTime>())
      {
        return databaseModel()->convertFromServer(var.value<QDateTime>());
      }
      else 
      {
        return QVariant(QVariant::DateTime);
      }
    }
    else if (4 == column || 5 == column || 6 == column)
    {        
      QVariant var = QSqlQueryModel::data(index, role);
      if (var.canConvert<QDate>())
      {
        return databaseModel()->convertFromServer(var.value<QDate>());
      }
      else 
      {
        return QVariant(QVariant::Date);
      }
    }
    else if (2 == column)
    {
      QVariant amountVar = QSqlQueryModel::data(index, role);          
      if (amountVar.canConvert<qint64>())                    
      {                        
        return QVariant::fromValue(
            model::CurrencyAmount(amountVar.toLongLong()));
      }
      else 
      {
        return QVariant();
      }
    }        
  }
  else if (Qt::TextAlignmentRole == role && index.isValid())
  {
    if (1 == column || 2 == column)
    {
      return QVariant(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
    }          
    else if (column)
    {
      return QVariant(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
    }
  }       
  return QSqlQueryModel::data(index, role);      
}

TableDescription ContractInsuranceTableModel::tableDescription()
{
  static TableDescription tableDescription;
  if (tableDescription.isEmpty())
  {
    for (int i = 0; i != selectedFieldCount; ++i)
    {
      tableDescription.addField(boost::make_shared<FieldDescription>(
          tr(selectedFieldTitles[i]), 
          selectedFieldNames[i], 
          selectedFieldNullability[i], 
          selectedFieldTypes[i], 
          selectedFieldEntitySubTypes[i]));
    }           
  }      
  return tableDescription;
}

void ContractInsuranceTableModel::setContractId(qint64 contractId)
{
  if (!contractId_)
  {
    contractId_ = contractId;
  }
}

QString ContractInsuranceTableModel::internalFilterSql() const
{
  if (contractId_)
  {
    const static QString contractFilterSql("t.\"CONTRACT_ID\" = ?");
    return contractFilterSql;
  }
  return QString();
}

int ContractInsuranceTableModel::internalFilterParamCount() const
{
  if (contractId_)
  {
    return 1;
  }
  return 0;
}

void ContractInsuranceTableModel::bindInternalFilterQueryParams(
    QSqlQuery& query, std::size_t baseParamNo) const
{
  if (contractId_)
  {
    query.bindValue(boost::numeric_cast<int>(baseParamNo), contractId_.get());
  }
}

QString ContractInsuranceTableModel::selectSql() const
{
  return selectPartSql;
}

} // namespace chrono
} //namespace ma

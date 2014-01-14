/*
TRANSLATOR ma::chrono::ContractPaysheetTableModel
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/make_shared.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <ma/chrono/dateutility.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/model/currencyamount.h>
#include <ma/chrono/contractpaysheettablemodel.h>

namespace ma {

namespace chrono {

namespace {    

const int queryColumnCount = 11;

const char* selectPartSql = 
  "select t.\"PAY_SHEET_ID\", " \
  "c.\"NUMBER\", t.\"DOC_NUMBER\", t.\"PAY_DATE\", t.\"AMOUNT\", " \
  "cur.\"ALPHABETIC_CODE\", t.\"ACCOUNT_NUMBER\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
  "from \"PAY_SHEET\" t " \
  "left join \"CCD\" c on t.\"CCD_ID\" = c.\"CCD_ID\" " \
  "join \"CURRENCY\" cur on t.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\"";

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "CCD number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "Document number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "Pay date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "Amount"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "Currency"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "Account number"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "User, last update"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "Time, last update")        
};      

const int selectedFieldCount = 10;

const char* selectedFieldNames[selectedFieldCount] = 
{        
  "c.\"NUMBER\"",
  "t.\"DOC_NUMBER\"",
  "t.\"ACCOUNT_NUMBER\"",        
  "c.\"CCD_ID\"",
  "cur.\"CURRENCY_ID\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"PAY_DATE\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{        
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "CCD number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "Document number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "Account number"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "CCD"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "Currency"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "Pay date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractPaysheetTableModel", "Time, last update")
};

const bool selectedFieldNullability[selectedFieldCount] = 
{        
  true,
  false,
  true,
  true,        
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
  FieldDescription::stringField,
  FieldDescription::stringField,
  FieldDescription::entityField,
  FieldDescription::entityField,
  FieldDescription::entityField,
  FieldDescription::entityField,        
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField
};

const Entity selectedFieldEntitySubTypes[selectedFieldCount] = 
{        
  unknownEntity,
  unknownEntity,
  unknownEntity,        
  ccdEntity,
  currencyEntity,
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity
};

} // namespace

ContractPaysheetTableModel::ContractPaysheetTableModel(
    const DatabaseModelPtr& databaseModel, QObject* parent,
    const OptionalQInt64& contractId, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)
  , contractId_(contractId)
{      
}

ContractPaysheetTableModel::~ContractPaysheetTableModel()
{
}    

int ContractPaysheetTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant ContractPaysheetTableModel::headerData(int section, 
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
      if (1 == section || 2 == section || 4 == section || 6 == section)
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

QVariant ContractPaysheetTableModel::data(
    const QModelIndex& index, int role) const
{
  int column = index.column();
  if (Qt::DisplayRole == role)
  {        
    if (10 == column || 9 == column)
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
    else if (3 == column)
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
    else if (4 == column)
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
    if (1 == column || 2 == column || 4 == column || 6 == column)
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

TableDescription ContractPaysheetTableModel::tableDescription()
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

void ContractPaysheetTableModel::setContractId(qint64 contractId)
{
  if (!contractId_)
  {
    contractId_ = contractId;
  }
}

QString ContractPaysheetTableModel::internalFilterSql() const
{
  if (contractId_)
  {
    const static QString contractFilterSql("t.\"CONTRACT_ID\" = ?");
    return contractFilterSql;
  }
  return QString();
}

int ContractPaysheetTableModel::internalFilterParamCount() const
{
  if (contractId_)
  {
    return 1;
  }
  return 0;
}

void ContractPaysheetTableModel::bindInternalFilterQueryParams(QSqlQuery& query,
    std::size_t baseParamNo) const
{
  if (contractId_)
  {
    query.bindValue(boost::numeric_cast<int>(baseParamNo), contractId_.get());
  }
}

QString ContractPaysheetTableModel::selectSql() const
{
  return selectPartSql;
}
        
} // namespace chrono
} // namespace ma

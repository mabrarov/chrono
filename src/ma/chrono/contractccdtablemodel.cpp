/*
TRANSLATOR ma::chrono::ContractCcdTableModel
*/

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/make_shared.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/model/currencyamount.h>
#include <ma/chrono/model/rateamount.h>
#include <ma/chrono/contractccdtablemodel.h>

namespace ma {

namespace chrono {

namespace {

const int queryColumnCount = 14;
const char* selectPartSql = 
  "select t.\"CCD_ID\", t.\"NUMBER\", t.\"AMOUNT\", " \
  "d.\"SHORT_NAME\", t.\"EXPORT_PERM_DATE\", tt.\"NAME\", t.\"DUTY\", t.\"RATE\", " \
  "t.\"PAYMENT_LIMIT_DATE\", t.\"EXPORT_DATE\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
  "from \"CCD_SEL\" t " \
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
  "join \"DEPPOINT\" d on t.\"DEPPOINT_ID\" = d.\"DEPPOINT_ID\" " \
  "join \"TTERM\" tt on t.\"TTERM_ID\" = tt.\"TTERM_ID\"";

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "CCD number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Amount"),      
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Deppoint"),      
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Export permission date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Transaction terms"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Duty"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Rate"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Payment limit date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Export date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Time, last update")
};

const int selectedFieldCount = 15;

const char* selectedFieldNames[selectedFieldCount] = 
{
  "t.\"NUMBER\"",        
  "d.\"DEPPOINT_ID\"",        
  "tt.\"TTERM_ID\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"EXPORT_PERM_DATE\"",      
  "t.\"EXPORT_DATE\"",
  "t.\"PAYMENT_LIMIT_DATE\"",
  "t.\"ZERO_TAX_LIMIT_DATE\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\"",
  "\"ICDOC_ID\"",
  "\"BOL_ID\"",
  "\"SBOL_ID\"",        
  "\"PAY_SHEET_ID\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "CCD number"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Deppoint"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Transaction terms"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Export permission date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Export date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Payment limit date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Zero tax limit date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "Time, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "List of inquiry on confirming documents"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "List of bills of lading"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "List of shipped bills of lading"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractCcdTableModel", "List of Pay Sheets")
};

const bool selectedFieldNullability[selectedFieldCount] = 
{      
  false,        
  false,        
  false,
  false,
  false,
  false,
  true,
  true,
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
  FieldDescription::entityField,
  FieldDescription::entityField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::entitySetField,
  FieldDescription::entitySetField,
  FieldDescription::entitySetField,
  FieldDescription::entitySetField
};

const Entity selectedFieldEntitySubTypes[selectedFieldCount] = 
{
  unknownEntity,
  deppointEntity,        
  ttermEntity,
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  icdocEntity,
  bolEntity,
  sbolEntity,
  paysheetEntity
};

const char* selectedFieldRelationFieldName[selectedFieldCount] =
{
  0,        
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,        
  "t.\"CCD_ID\"",
  "t.\"CCD_ID\"",
  "t.\"CCD_ID\"",
  "t.\"CCD_ID\""
};

const char* selectedFieldEntitySetTableName[selectedFieldCount] =
{
  0,
  0,
  0,        
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,     
  "\"CCD_ICDOC_RELATION\"",      
  "\"BOL\"",
  "\"SBOL\"",
  "\"PAY_SHEET\""
};

const char* selectedFieldEntitySetRelationFieldName[selectedFieldCount] = 
{
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,     
  "\"CCD_ID\"",
  "\"CCD_ID\"",
  "\"CCD_ID\"",
  "\"CCD_ID\""
};

} // namespace

ContractCcdTableModel::ContractCcdTableModel(
    const DatabaseModelPtr& databaseModel, 
    QObject* parent, 
    const OptionalQInt64& contractId, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)
  , contractId_(contractId)
{      
}

ContractCcdTableModel::~ContractCcdTableModel()
{
}    

int ContractCcdTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant ContractCcdTableModel::headerData(int section, 
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
      if (1 == section || 2 == section || 6 == section || 7 == section)
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

QVariant ContractCcdTableModel::data(const QModelIndex& index, int role) const
{
  int column = index.column();
  if (Qt::DisplayRole == role)
  {        
    if (12 == column || 13 == column)
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
    else if (4 == column || 8 == column || 9 == column)
    {        
      QVariant var = QSqlQueryModel::data(index, role);
      if (!var.isNull() && var.canConvert<QDate>())
      {
        return databaseModel()->convertFromServer(var.value<QDate>());
      }
      else 
      {
        return QVariant(QVariant::Date);
      }
    }
    else if (2 == column || 6 == column)
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
    else if (7 == column)
    {
      QVariant amountVar = QSqlQueryModel::data(index, role);          
      if (amountVar.canConvert<qint64>())                    
      {               
        return QVariant::fromValue(model::RateAmount(amountVar.toLongLong()));
      }
      else 
      {
        return QVariant();
      }
    }
  }
  else if (Qt::TextAlignmentRole == role && index.isValid())
  {
    if (1 == column || 2 == column || 6 == column || 7 == column)
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

TableDescription ContractCcdTableModel::tableDescription()
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
          selectedFieldEntitySubTypes[i],
          selectedFieldEntitySetTableName[i] 
              ? QString(selectedFieldEntitySetTableName[i]) : QString(),
          selectedFieldRelationFieldName[i]
              ? QString(selectedFieldRelationFieldName[i]) : QString(),
          selectedFieldEntitySetRelationFieldName[i] 
              ? QString(selectedFieldEntitySetRelationFieldName[i]) : QString()
          ));
    }           
  }      
  return tableDescription;
} 

void ContractCcdTableModel::setContractId(qint64 contractId)
{
  if (!contractId_)
  {
    contractId_ = contractId;
  }
}

QString ContractCcdTableModel::internalFilterSql() const
{
  if (contractId_)
  {
    const static QString contractFilterSql("t.\"CONTRACT_ID\" = ?");
    return contractFilterSql;
  }
  return QString();
}

int ContractCcdTableModel::internalFilterParamCount() const
{
  if (contractId_)
  {
    return 1;
  }
  return 0;
}

void ContractCcdTableModel::bindInternalFilterQueryParams(QSqlQuery& query, 
    std::size_t baseParamNo) const
{
  if (contractId_)
  {
    query.bindValue(boost::numeric_cast<int>(baseParamNo), contractId_.get());
  }
}

QString ContractCcdTableModel::selectSql() const
{
  return selectPartSql;
}
        
} // namespace chrono
} // namespace ma
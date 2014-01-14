/*
TRANSLATOR ma::chrono::ContractSbolTableModel
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
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/contractsboltablemodel.h>

namespace ma {

namespace chrono {

namespace {

const int queryColumnCount = 9;
const char* selectPartSql = 
  "select t.\"SBOL_ID\", " \
  "t.\"NUMBER\", t.\"SBOL_DATE\", ccd.\"NUMBER\", l.\"NUMBER\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
  "from \"SBOL\" t " \
  "join \"CCD\" ccd on t.\"CCD_ID\" = ccd.\"CCD_ID\" " \
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
  "left join \"LCREDIT\" l on t.\"LCREDIT_ID\" = l.\"LCREDIT_ID\"";

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "Shipped bill of lading number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "Date of shipped bill of lading"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "CCD number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "Number of letter of credit"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "User, last update"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "Time, last update")        
};

const int selectedFieldCount = 8;

const char* selectedFieldNames[selectedFieldCount] = 
{
  "t.\"NUMBER\"",
  "ccd.\"CCD_ID\"",
  "l.\"LCREDIT_ID\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"SBOL_DATE\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "Shipped bill of lading number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "CCD"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "Letter of credit"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "Date of shipped bill of lading"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractSbolTableModel", "Time, last update")        
};

const bool selectedFieldNullability[selectedFieldCount] = 
{
  false,
  false,
  true,
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
  FieldDescription::dateField
};

const Entity selectedFieldEntitySubTypes[selectedFieldCount] = 
{
  unknownEntity,        
  ccdEntity,
  lcreditEntity,
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity
};

} // namespace

ContractSbolTableModel::ContractSbolTableModel(
    const DatabaseModelPtr& databaseModel, 
    QObject* parent, 
    const OptionalQInt64& contractId, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)
  , contractId_(contractId)
{      
}

ContractSbolTableModel::~ContractSbolTableModel()
{
}    

int ContractSbolTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant ContractSbolTableModel::headerData(int section, 
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
      if (1 == section || 3 == section || 4 == section)
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

QVariant ContractSbolTableModel::data(const QModelIndex& index, int role) const
{
  int column = index.column();
  if (Qt::DisplayRole == role)
  {        
    if (7 == column || 8 == column)
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
    else if (2 == column)
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
  }
  else if (Qt::TextAlignmentRole == role && index.isValid())
  {
    if (1 == column || 3 == column || 4 == column)
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

TableDescription ContractSbolTableModel::tableDescription()
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

void ContractSbolTableModel::setContractId(qint64 contractId)
{
  if (!contractId_)
  {
    contractId_ = contractId;
  }
}

QString ContractSbolTableModel::internalFilterSql() const
{
  if (contractId_)
  {
    const static QString contractFilterSql("ccd.\"CONTRACT_ID\" = ?");
    return contractFilterSql;
  }
  return QString();
}

int ContractSbolTableModel::internalFilterParamCount() const
{
  if (contractId_)
  {
    return 1;
  }
  return 0;
}

void ContractSbolTableModel::bindInternalFilterQueryParams(QSqlQuery& query,
    std::size_t baseParamNo) const
{
  if (contractId_)
  {
    query.bindValue(boost::numeric_cast<int>(baseParamNo), contractId_.get());
  }
}

QString ContractSbolTableModel::selectSql() const
{
  return selectPartSql;
}
        
} // namespace chrono
} // namespace ma

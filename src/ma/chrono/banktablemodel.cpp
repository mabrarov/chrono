/*
TRANSLATOR ma::chrono::BankTableModel
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/make_shared.hpp>
#include <QSqlQuery>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/banktablemodel.h>

namespace ma {
namespace chrono {

namespace {    

const int queryColumnCount = 9;   
const char* selectPartSql = "select " \
    "t.\"BANK_ID\", t.\"BANK_CODE\", t.\"FULL_NAME\", t.\"SHORT_NAME\", " \
    "t.\"REMARK\", cu.\"LOGIN\", uu.\"LOGIN\", " \
    "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
    "from \"BANK\" t "
    "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
    "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\""; 

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::BankTableModel", "No."),      
  QT_TRANSLATE_NOOP("ma::chrono::BankTableModel", "Bank code"),
  QT_TRANSLATE_NOOP("ma::chrono::BankTableModel", "Full name"),
  QT_TRANSLATE_NOOP("ma::chrono::BankTableModel", "Short name"),        
  QT_TRANSLATE_NOOP("ma::chrono::BankTableModel", "Remarks"),        
  QT_TRANSLATE_NOOP("ma::chrono::BankTableModel", "User, creator"),        
  QT_TRANSLATE_NOOP("ma::chrono::BankTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::BankTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::BankTableModel", "Time, last update")
};

const int selectedFieldCount = 7;

const char* selectedFieldNames[selectedFieldCount] = 
{      
  "t.\"FULL_NAME\"", 
  "t.\"SHORT_NAME\"", 
  "t.\"REMARK\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{            
  QT_TRANSLATE_NOOP("ma::chrono::BankTableModel", "Full name"),
  QT_TRANSLATE_NOOP("ma::chrono::BankTableModel", "Short name"),        
  QT_TRANSLATE_NOOP("ma::chrono::BankTableModel", "Remarks"),        
  QT_TRANSLATE_NOOP("ma::chrono::BankTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::BankTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::BankTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::BankTableModel", "Time, last update")
};

const bool selectedFieldNullability[selectedFieldCount] = 
{
  false,
  false,
  true,
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
  FieldDescription::dateField,
  FieldDescription::dateField
};

const Entity selectedFieldEntitySubTypes[selectedFieldCount] = 
{      
  unknownEntity,
  unknownEntity,
  unknownEntity,
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity
};

} // anonymous namespace

BankTableModel::BankTableModel(const DatabaseModelPtr& databaseModel, 
    QObject* parent, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)    
{      
}

BankTableModel::~BankTableModel()
{
}    

int BankTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant BankTableModel::headerData(int section, 
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
      if (1 == section)
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

QVariant BankTableModel::data(const QModelIndex& index, int role) const
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
  }
  else if (Qt::TextAlignmentRole == role && index.isValid())
  {
    if (1 == column)
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
    
TableDescription BankTableModel::tableDescription()
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

QString BankTableModel::selectSql() const
{
  return selectPartSql;
}      

} // namespace chrono
} // namespace ma

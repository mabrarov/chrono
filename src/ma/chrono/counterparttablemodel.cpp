/*
TRANSLATOR ma::chrono::CounterpartTableModel
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
#include <ma/chrono/counterparttablemodel.h>

namespace ma {

namespace chrono {

namespace {

const int queryColumnCount = 10;   
const char* selectPartSql = "select " \
  "t.\"COUNTERPART_ID\", t.\"FULL_NAME\", t.\"FULL_LATIN_NAME\", " \
  "t.\"SHORT_NAME\", t.\"SHORT_LATIN_NAME\", " \
  "cn.\"SHORT_NAME\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
  "from \"COUNTERPART\" t " \
  "join \"COUNTRY\" cn on t.\"COUNTRY_ID\" = cn.\"COUNTRY_ID\" " \
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\"";

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "Full name"),
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "Full latin name"),
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "Short name"),
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "Short latin name"),                
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "Country"),        
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "User, creator"),        
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "Time, last update")
};
        
const int selectedFieldCount = 9;

const char* selectedFieldNames[selectedFieldCount] = 
{
  "t.\"FULL_NAME\"",
  "t.\"FULL_LATIN_NAME\"",
  "t.\"SHORT_NAME\"",
  "t.\"SHORT_LATIN_NAME\"",
  "cn.\"COUNTRY_ID\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "Full name"),
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "Full latin name"),
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "Short name"),
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "Short latin name"),
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "Country"),      
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::CounterpartTableModel", "Time, last update")
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
  false,
  false      
};

const FieldDescription::FieldType selectedFieldTypes[selectedFieldCount] = 
{      
  FieldDescription::stringField,
  FieldDescription::stringField,
  FieldDescription::stringField,
  FieldDescription::stringField,
  FieldDescription::entityField,
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
  unknownEntity,
  countryEntity,      
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity
};

} // namespace

CounterpartTableModel::CounterpartTableModel(
    const DatabaseModelPtr& databaseModel, 
    QObject* parent, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)
{      
}

CounterpartTableModel::~CounterpartTableModel()
{
}    

int CounterpartTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant CounterpartTableModel::headerData(int section, 
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
      if (section)
      {
        return QVariant(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
      }
    }
  }
  return QSqlQueryModel::headerData(section, orientation, role);
}

QVariant CounterpartTableModel::data(const QModelIndex& index, int role) const
{
  int column = index.column();
  if (Qt::DisplayRole == role)
  {        
    if (8 == column || 9 == column)
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
    if (column)
    {
      return QVariant(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
    }
  }
  return QSqlQueryModel::data(index, role);      
}
    
TableDescription CounterpartTableModel::tableDescription()
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

QString CounterpartTableModel::selectSql() const
{
  return selectPartSql;
}       

} // namespace chrono
} // namespace ma

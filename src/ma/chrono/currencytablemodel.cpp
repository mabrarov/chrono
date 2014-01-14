/*
TRANSLATOR ma::chrono::CurrencyTableModel
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
#include <ma/chrono/currencytablemodel.h>

namespace ma {

namespace chrono {

namespace {

const int queryColumnCount = 8;   
const char* selectPartSql = "select " \
  "c.\"CURRENCY_ID\", c.\"FULL_NAME\", c.\"ALPHABETIC_CODE\", c.\"NUMERIC_CODE\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "c.\"CREATE_TIME\", c.\"UPDATE_TIME\" " \
  "from \"CURRENCY\" c " \
  "join \"USER\" cu on c.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on c.\"UPDATE_USER_ID\" = uu.\"USER_ID\""; 

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::CurrencyTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::CurrencyTableModel", "Full name"),
  QT_TRANSLATE_NOOP("ma::chrono::CurrencyTableModel", "Alphabetic code"),
  QT_TRANSLATE_NOOP("ma::chrono::CurrencyTableModel", "Numeric code"),        
  QT_TRANSLATE_NOOP("ma::chrono::CurrencyTableModel", "User, creator"),        
  QT_TRANSLATE_NOOP("ma::chrono::CurrencyTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::CurrencyTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::CurrencyTableModel", "Time, last update")
};

const int selectedFieldCount = 6;

const char* selectedFieldNames[selectedFieldCount] = 
{      
  "c.\"FULL_NAME\"",
  "c.\"ALPHABETIC_CODE\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{      
  QT_TRANSLATE_NOOP("ma::chrono::CurrencyTableModel", "Full name"),
  QT_TRANSLATE_NOOP("ma::chrono::CurrencyTableModel", "Alphabetic code"),
  QT_TRANSLATE_NOOP("ma::chrono::CurrencyTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::CurrencyTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::CurrencyTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::CurrencyTableModel", "Time, last update")
};

const bool selectedFieldNullability[selectedFieldCount] = 
{
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
  FieldDescription::entityField,
  FieldDescription::entityField,
  FieldDescription::dateField,
  FieldDescription::dateField
};

const Entity selectedFieldEntitySubTypes[selectedFieldCount] = 
{      
  unknownEntity,
  unknownEntity,
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity
};

} // namespace

CurrencyTableModel::CurrencyTableModel(const DatabaseModelPtr& databaseModel,
    QObject* parent, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)    
{      
}

CurrencyTableModel::~CurrencyTableModel()
{
}    

int CurrencyTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant CurrencyTableModel::headerData(int section, 
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
      if (3 == section)
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

QVariant CurrencyTableModel::data(const QModelIndex& index, int role) const
{
  int column = index.column();
  if (Qt::DisplayRole == role)
  {        
    if (6 == column || 7 == column)
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
    if (3 == column)
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
    
TableDescription CurrencyTableModel::tableDescription()
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

QString CurrencyTableModel::selectSql() const
{
  return selectPartSql;
}
        
} // namespace chrono
} // namespace ma

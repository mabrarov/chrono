/*
TRANSLATOR ma::chrono::UserTableModel
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
#include <ma/chrono/usertablemodel.h>

namespace ma {

namespace chrono {

namespace {

const int queryColumnCount = 9;   
const char* selectPartSql = "select " \
  "u.\"USER_ID\", u.\"LOGIN\", u.\"SURNAME\", " \
  "u.\"FIRST_NAME\", u.\"SECOND_NAME\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "u.\"CREATE_TIME\", u.\"UPDATE_TIME\" " \
  "from \"USER\" u " \
  "left join \"USER\" cu on u.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "left join \"USER\" uu on u.\"UPDATE_USER_ID\" = uu.\"USER_ID\"";

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "User name"),
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "Last name"),
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "First name"),
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "Middle name"),
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "User, creator"),        
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "Time, last update")
};

const int selectedFieldCount = 8;

const char* selectedFieldNames[selectedFieldCount] = 
{
  "u.\"LOGIN\"",
  "u.\"SURNAME\"",
  "u.\"FIRST_NAME\"", 
  "u.\"SECOND_NAME\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "User name"),
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "Last name"),
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "First name"),
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "Middle name"),
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::UserTableModel", "Time, last update")
};
      
const bool selectedFieldNullability[selectedFieldCount] = 
{      
  false,
  false,
  false,
  true,
  true,
  true,
  true,
  true
};

const FieldDescription::FieldType selectedFieldTypes[selectedFieldCount] =
{      
  FieldDescription::stringField,
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
  unknownEntity,
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity
};

} // namespace

UserTableModel::UserTableModel(const DatabaseModelPtr& databaseModel, 
    QObject* parent, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)    
{      
}

UserTableModel::~UserTableModel()
{
}    

int UserTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant UserTableModel::headerData(int section, 
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

QVariant UserTableModel::data(const QModelIndex& index, int role) const
{      
  if (Qt::DisplayRole == role)
  {
    int column = index.column();
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
    if (index.column())
    {
      return QVariant(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
    }          
  }
  return QSqlQueryModel::data(index, role);
}
    
TableDescription UserTableModel::tableDescription()
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

QString UserTableModel::selectSql() const
{
  return selectPartSql;
}        

} // namespace chrono
} // namespace ma

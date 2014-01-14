/*
TRANSLATOR ma::chrono::AutoenterpriseTableModel
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
#include <ma/chrono/autoenterprisetablemodel.h>

namespace ma {
namespace chrono {

namespace {

const int queryColumnCount = 8;   
    
const char* selectPartSql = "select " \
    "t.\"AUTOENTERPRISE_ID\", t.\"FULL_NAME\", t.\"SHORT_NAME\", " \
    "t.\"REMARK\", cu.\"LOGIN\", uu.\"LOGIN\", " \
    "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
    "from \"AUTOENTERPRISE\" t "
    "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
    "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\""; 

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::AutoenterpriseTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::AutoenterpriseTableModel", "Full name"),
  QT_TRANSLATE_NOOP("ma::chrono::AutoenterpriseTableModel", "Short name"),        
  QT_TRANSLATE_NOOP("ma::chrono::AutoenterpriseTableModel", "Remarks"),        
  QT_TRANSLATE_NOOP("ma::chrono::AutoenterpriseTableModel", "User, creator"),        
  QT_TRANSLATE_NOOP("ma::chrono::AutoenterpriseTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::AutoenterpriseTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::AutoenterpriseTableModel", "Time, last update")
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
  QT_TRANSLATE_NOOP("ma::chrono::AutoenterpriseTableModel", "Full name"),
  QT_TRANSLATE_NOOP("ma::chrono::AutoenterpriseTableModel", "Short name"),        
  QT_TRANSLATE_NOOP("ma::chrono::AutoenterpriseTableModel", "Remarks"),
  QT_TRANSLATE_NOOP("ma::chrono::AutoenterpriseTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::AutoenterpriseTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::AutoenterpriseTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::AutoenterpriseTableModel", "Time, last update")
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

} // namespace

AutoenterpriseTableModel::AutoenterpriseTableModel(
    const DatabaseModelPtr& databaseModel, 
    QObject* parent, const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)
{      
}

AutoenterpriseTableModel::~AutoenterpriseTableModel()
{
}    

int AutoenterpriseTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant AutoenterpriseTableModel::headerData(int section, 
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

QVariant AutoenterpriseTableModel::data(
    const QModelIndex& index, int role) const
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
    if (column)
    {
      return QVariant(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
    }
  }
  return QSqlQueryModel::data(index, role);      
}
    
TableDescription AutoenterpriseTableModel::tableDescription()
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

QString AutoenterpriseTableModel::selectSql() const
{
  return selectPartSql;
}    

} // namespace chrono
} // namespace ma

/*
TRANSLATOR ma::chrono::DeppointTableModel
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <boost/make_shared.hpp>
#include <QSqlQuery>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/deppointtablemodel.h>

namespace ma {

namespace chrono {

namespace {

const int queryColumnCount = 8;   
const char* selectPartSql = "select " 
  "t.\"DEPPOINT_ID\", t.\"FULL_NAME\", t.\"SHORT_NAME\", t.\"REMARK\", "
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
  "from \"DEPPOINT\" t "
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\""; 

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::DeppointTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::DeppointTableModel", "Full name"),
  QT_TRANSLATE_NOOP("ma::chrono::DeppointTableModel", "Short name"),        
  QT_TRANSLATE_NOOP("ma::chrono::DeppointTableModel", "Remarks"),        
  QT_TRANSLATE_NOOP("ma::chrono::DeppointTableModel", "User, creator"),        
  QT_TRANSLATE_NOOP("ma::chrono::DeppointTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::DeppointTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::DeppointTableModel", "Time, last update")
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
  QT_TRANSLATE_NOOP("ma::chrono::DeppointTableModel", "Full name"),
  QT_TRANSLATE_NOOP("ma::chrono::DeppointTableModel", "Short name"),        
  QT_TRANSLATE_NOOP("ma::chrono::DeppointTableModel", "Remarks"),        
  QT_TRANSLATE_NOOP("ma::chrono::DeppointTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::DeppointTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::DeppointTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::DeppointTableModel", "Time, last update")
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

DeppointTableModel::DeppointTableModel(const DatabaseModelPtr& databaseModel,
    QObject* parent, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)    
{      
}

DeppointTableModel::~DeppointTableModel()
{
}    

int DeppointTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant DeppointTableModel::headerData(int section, 
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

QVariant DeppointTableModel::data(const QModelIndex& index, int role) const
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
    
TableDescription DeppointTableModel::tableDescription()
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

QString DeppointTableModel::selectSql() const
{
  return selectPartSql;
}       

} // namespace chrono
} //namespace ma

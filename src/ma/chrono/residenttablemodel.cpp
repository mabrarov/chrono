/*
TRANSLATOR ma::chrono::ResidentTableModel
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <boost/make_shared.hpp>
#include <QSqlQuery>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/residenttablemodel.h>

namespace ma {

namespace chrono {

namespace {    

const int queryColumnCount = 11;   
const char* selectPartSql = "select " \
  "t.\"RESIDENT_ID\", t.\"FULL_NAME\", t.\"SHORT_NAME\", t.\"REGISTRY_NUMBER\", t.\"INTB\", t.\"CRSA\", " \
  "cn.\"SHORT_NAME\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
  "from \"RESIDENT\" t " \
  "join \"COUNTRY\" cn on t.\"COUNTRY_ID\" = cn.\"COUNTRY_ID\" " \
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\""; 

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "Full name"),
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "Short name"),
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "Registry number"),
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "INTB"),
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "CRSA"),        
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "Country"),        
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "User, creator"),        
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "Time, last update")
};

const int selectedFieldCount = 15;

const char* selectedFieldNames[selectedFieldCount] = 
{
  "t.\"FULL_NAME\"",
  "t.\"SHORT_NAME\"",
  "t.\"REGISTRY_NUMBER\"",
  "t.\"INTB\"",
  "t.\"CRSA\"",
  "t.\"REGION_NAME\"",
  "t.\"CITY_NAME\"",
  "t.\"STREET_NAME\"",
  "t.\"ZIP_CODE\"",
  "cn.\"COUNTRY_ID\"",      
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"SRE_DATE\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "Full name"),
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "Short name"),
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "Registry number"),
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "INTB"),
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "CRSA"),        
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "Region name"),        
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "City name"),
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "Street name"),
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "ZIP code"),
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "Country"),        
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "User, creator"),        
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "State registry entering date"),      
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::ResidentTableModel", "Time, last update")
};

const bool selectedFieldNullability[selectedFieldCount] = 
{
  false,
  false,
  true,
  true,
  true,
  true,
  true,
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
  FieldDescription::stringField,
  FieldDescription::stringField,
  FieldDescription::stringField,
  FieldDescription::stringField,
  FieldDescription::stringField,
  FieldDescription::stringField,
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
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  countryEntity,      
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity
};

} // namespace

ResidentTableModel::ResidentTableModel(const DatabaseModelPtr& databaseModel, 
    QObject* parent, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)    
{      
}

ResidentTableModel::~ResidentTableModel()
{
}    

int ResidentTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant ResidentTableModel::headerData(int section, 
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
      if (3 == section || 4 == section || 5 == section)
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

QVariant ResidentTableModel::data(const QModelIndex& index, int role) const
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
  }
  else if (Qt::TextAlignmentRole == role && index.isValid())
  {
    if (3 == column || 4 == column || 5 == column)
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
    
TableDescription ResidentTableModel::tableDescription()
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

QString ResidentTableModel::selectSql() const
{
  return selectPartSql;
}       

} // namespace chrono
} //namespace ma

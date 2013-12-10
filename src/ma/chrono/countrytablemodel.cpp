/*
TRANSLATOR ma::chrono::CountryTableModel
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <boost/make_shared.hpp>
#include <QSqlQuery>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/countrytablemodel.h>

namespace ma {

namespace chrono {

namespace {

const int queryColumnCount = 11;   
const char* selectPartSql = "select " \
  "c.\"COUNTRY_ID\", c.\"FULL_NAME\", c.\"SHORT_NAME\", " \
  "c.\"TRIGRAM_CYRILLIC_CODE\", c.\"TRIGRAM_LATIN_CODE\", " \
  "c.\"TWO_LETTER_LATIN_CODE\", c.\"NUMERIC_CODE\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "c.\"CREATE_TIME\", c.\"UPDATE_TIME\" " \
  "from \"COUNTRY\" c " \
  "join \"USER\" cu on c.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on c.\"UPDATE_USER_ID\" = uu.\"USER_ID\""; 

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "Full name"),
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "Short name"),
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "Trigram cyrillic code"),
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "Trigram latin code"),
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "Two letter latincode"),
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "Numeric code"),        
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "User, creator"),        
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "Time, last update")
};

const int selectedFieldCount = 9;

const char* selectedFieldNames[selectedFieldCount] = 
{      
  "c.\"FULL_NAME\"",
  "c.\"SHORT_NAME\"",
  "c.\"TRIGRAM_CYRILLIC_CODE\"", 
  "c.\"TRIGRAM_LATIN_CODE\"",
  "c.\"TWO_LETTER_LATIN_CODE\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{      
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "Full name"),
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "Short name"),
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "Trigram cyrillic code"),
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "Trigram latin code"),
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "Two letter latincode"),
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::CountryTableModel", "Time, last update")
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
  unknownEntity,
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity
};

} // namespace

CountryTableModel::CountryTableModel(const DatabaseModelPtr& databaseModel,
    QObject* parent, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)
{      
}

CountryTableModel::~CountryTableModel()
{
}    

int CountryTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant CountryTableModel::headerData(int section, 
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
      if (6 == section)
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

QVariant CountryTableModel::data(const QModelIndex& index, int role) const
{
  int column = index.column();
  if (Qt::DisplayRole == role)
  {        
    if (9 == column || 10 == column)
    {        
      QVariant var = QSqlQueryModel::data(index, role);
      if (var.canConvert<QDateTime>())
      {
        return databaseModel()->convertFromServer(var.value<QDateTime>());
      }
    }
  }
  else if (Qt::TextAlignmentRole == role && index.isValid())
  {
    if (6 == column)
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
    
TableDescription CountryTableModel::tableDescription()
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

QString CountryTableModel::selectSql() const
{
  return selectPartSql;
}        

} // namespace chrono
} //namespace ma

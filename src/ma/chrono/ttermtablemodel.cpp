/*
TRANSLATOR ma::chrono::TtermTableModel
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <boost/make_shared.hpp>
#include <QSqlQuery>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/ttermtablemodel.h>

namespace ma {

namespace chrono {

namespace {

const int queryColumnCount = 7;   
const char* selectPartSql = "select " 
  "t.\"TTERM_ID\", t.\"NAME\", t.\"REMARK\", "
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
  "from \"TTERM\" t "            
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\"";

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::TtermTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::TtermTableModel", "Name"),
  QT_TRANSLATE_NOOP("ma::chrono::TtermTableModel", "Remarks"),        
  QT_TRANSLATE_NOOP("ma::chrono::TtermTableModel", "User, creator"),        
  QT_TRANSLATE_NOOP("ma::chrono::TtermTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::TtermTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::TtermTableModel", "Time, last update")
};

const int selectedFieldCount = 6;

const char* selectedFieldNames[selectedFieldCount] = 
{
  "t.\"NAME\"",
  "t.\"REMARK\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::TtermTableModel", "Name"),
  QT_TRANSLATE_NOOP("ma::chrono::TtermTableModel", "Remarks"),        
  QT_TRANSLATE_NOOP("ma::chrono::TtermTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::TtermTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::TtermTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::TtermTableModel", "Time, last update")
};

const bool selectedFieldNullability[selectedFieldCount] = 
{
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

TtermTableModel::TtermTableModel(const DatabaseModelPtr& databaseModel, 
    QObject* parent, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)    
{      
}

TtermTableModel::~TtermTableModel()
{
}    

int TtermTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant TtermTableModel::headerData(int section, 
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

QVariant TtermTableModel::data(const QModelIndex& index, int role) const
{      
  if (Qt::DisplayRole == role)
  {
    int column = index.column();
    if (5 == column || 6 == column)
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
    
TableDescription TtermTableModel::tableDescription()
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

QString TtermTableModel::selectSql() const
{
  return selectPartSql;
}        

} // namespace chrono
} //namespace ma

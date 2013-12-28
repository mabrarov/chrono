/*
TRANSLATOR ma::chrono::IcdocTableModel
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <boost/make_shared.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <ma/chrono/dateutility.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/model/currencyamount.h>
#include <ma/chrono/icdoctablemodel.h>

namespace ma {

namespace chrono {

namespace {

const int queryColumnCount = 9;    

const char* selectPartSql = 
  "select t.\"ICDOC_ID\", " \
  "b.\"SHORT_NAME\", t.\"ICDOC_DATE\", t.\"STATE\", t.\"RECV_DATE\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
  "from \"ICDOC\" t " \
  "join \"BANK\" b on t.\"BANK_ID\" = b.\"BANK_ID\" " \
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\""; 

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "No."),
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "Bank"),
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "Inquiry date"),
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "Receive confirmed by bank"),
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "Receive from bank date"),
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "User, last update"),        
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "Time, last update")        
};
      
const char* staticFilters[IcdocTableModel::filterIdCount] = 
{
  "t.\"STATE\" = 0",
  "t.\"STATE\" <> 0"
};

const int staticFilterParamCount[IcdocTableModel::filterIdCount] = 
{      
  0,      
  0      
};

const int selectedFieldCount = 8;

const char* selectedFieldNames[selectedFieldCount] = 
{      
  "b.\"BANK_ID\"",     
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"ICDOC_DATE\"",
  "t.\"RECV_DATE\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\"",
  "\"CCD_ID\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{      
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "Bank"),      
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "Inquiry date"),
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "Receive from bank date"),
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "Time, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::IcdocTableModel", "CCD list")
};

const bool selectedFieldNullability[selectedFieldCount] = 
{
  false,
  false,
  false,
  false,
  true,
  false,      
  false,
  false
};

const FieldDescription::FieldType selectedFieldTypes[selectedFieldCount] = 
{      
  FieldDescription::entityField,
  FieldDescription::entityField,
  FieldDescription::entityField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::entitySetField
};

const Entity selectedFieldEntitySubTypes[selectedFieldCount] = 
{      
  bankEntity,     
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  ccdEntity
};

const char* selectedFieldRelationFieldName[selectedFieldCount] =
{
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  "t.\"ICDOC_ID\""
};

const char* selectedFieldEntitySetTableName[selectedFieldCount] =
{
  0,
  0,
  0,
  0,
  0,
  0,
  0,      
  "\"CCD_ICDOC_RELATION\"",      
};

const char* selectedFieldEntitySetRelationFieldName[selectedFieldCount] = 
{
  0,
  0,
  0,
  0,
  0,
  0,
  0,      
  "\"ICDOC_ID\""
};      

} // namespace

IcdocTableModel::IcdocTableModel(const DatabaseModelPtr& databaseModel, 
    QObject* parent,
    const OptionalInt& filterId, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)
  , filterId_(filterId)
{
}

IcdocTableModel::~IcdocTableModel()
{
}    

int IcdocTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant IcdocTableModel::headerData(int section, 
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
      return QVariant(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
    }
  }
  return QSqlQueryModel::headerData(section, orientation, role);
}

QVariant IcdocTableModel::data(const QModelIndex& index, int role) const
{
  int column = index.column();
  if (Qt::DisplayRole == role)
  {        
    if (8 == column || 7 == column)
    {        
      QVariant var = QSqlQueryModel::data(index, role);
      if (var.canConvert<QDateTime>())
      {
        return databaseModel()->convertFromServer(var.value<QDateTime>());
      }
    }
    else if (2 == column || 4 == column)
    {        
      QVariant var = QSqlQueryModel::data(index, role);
      if (var.canConvert<QDate>())
      {
        return databaseModel()->convertFromServer(var.value<QDate>());
      }
    }
    else if (3 == column)
    {
      return QVariant();
    }
  }
  else if (Qt::TextAlignmentRole == role && index.isValid())
  {      
    return QVariant(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));      
  } 
  else if (Qt::CheckStateRole == role && 3 == column)
  {
    return QSqlQueryModel::data(index).toInt() != 0 
        ? Qt::Checked : Qt::Unchecked;
  }
  return QSqlQueryModel::data(index, role);      
}

TableDescription IcdocTableModel::tableDescription()
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
          selectedFieldEntitySubTypes[i],
          selectedFieldEntitySetTableName[i] 
              ? QString(selectedFieldEntitySetTableName[i]) : QString(),
          selectedFieldRelationFieldName[i] 
              ? QString(selectedFieldRelationFieldName[i]) : QString(),
          selectedFieldEntitySetRelationFieldName[i] 
              ? QString(selectedFieldEntitySetRelationFieldName[i]) : QString()
          ));
    }           
  }      
  return tableDescription;
}

QString IcdocTableModel::internalFilterSql() const
{
  if (filterId_)
  {
    return staticFilters[filterId_.get()];
  }
  return QString();      
}

int IcdocTableModel::internalFilterParamCount() const
{
  if (filterId_)
  {
    return staticFilterParamCount[filterId_.get()];
  }
  return 0;      
}

void IcdocTableModel::bindInternalFilterQueryParams(QSqlQuery& /*query*/, 
    std::size_t /*baseParamNo*/) const
{
  //if (filterId_)
  //{
  //  
  //} 
}

QString IcdocTableModel::selectSql() const
{
  return selectPartSql;
}       

} // namespace chrono
} //namespace ma
/*
TRANSLATOR ma::chrono::LcreditTableModel
*/

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/make_shared.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <ma/chrono/dateutility.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/model/currencyamount.h>
#include <ma/chrono/lcredittablemodel.h>

namespace ma {

namespace chrono {

namespace {    

const int queryColumnCount = 10;
const char* selectPartSql = 
  "select t.\"LCREDIT_ID\", cr.\"NUMBER\", cntp.\"SHORT_NAME\", " \
  "t.\"NUMBER\", t.\"LCREDIT_DATE\", t.\"END_DATE\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
  "from \"LCREDIT\" t " \
  "join \"CONTRACT\" cr on t.\"CONTRACT_ID\" = cr.\"CONTRACT_ID\" " \
  "join \"COUNTERPART\" cntp on cr.\"COUNTERPART_ID\" = cntp.\"COUNTERPART_ID\" " \
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\"";      

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "Contract number"),
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "Counterpart"),
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "Number of letter of credit"),
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "Date of letter of credit"),
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "End date of letter of credit"),        
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "User, last update"),        
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "Time, last update")        
};
      
const char* staticFilters[LcreditTableModel::filterIdCount] = 
{      
  "t.\"END_DATE\" < ?",      
  "t.\"END_DATE\" = ?",
  "t.\"END_DATE\" = ?",
  "t.\"END_DATE\" >= ? and t.\"END_DATE\" <= ?",
  "t.\"END_DATE\" >= ? and t.\"END_DATE\" <= ?",
  "t.\"END_DATE\" >= ? and t.\"END_DATE\" <= ?",
};

const int staticFilterParamCount[LcreditTableModel::filterIdCount] = 
{      
  1,
  1,            
  1,
  2,
  2,
  2
};

const int selectedFieldCount = 11;

const char* selectedFieldNames[selectedFieldCount] = 
{
  "cr.\"NUMBER\"",
  "t.\"NUMBER\"",
  "t.\"REMARK\"",
  "cr.\"CONTRACT_ID\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"LCREDIT_DATE\"", 
  "t.\"END_DATE\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\"",
  "\"SBOL_ID\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "Contract number"),      
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "Number of letter of credit"),
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "Remarks"),      
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "Contract"),
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "Date of letter of credit"),
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "End date of letter of credit"),              
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "Time, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::LcreditTableModel", "List of shipped bills of lading"),
};

const bool selectedFieldNullability[selectedFieldCount] = 
{
  false,
  false,
  true,
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
  unknownEntity,
  unknownEntity,
  unknownEntity,
  contractEntity,
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  sbolEntity
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
  0,
  0,
  0,
  "t.\"LCREDIT_ID\""        
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
  0,
  0,
  0,        
  "\"SBOL\""        
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
  0,
  0,
  0,        
  "\"LCREDIT_ID\""        
};

} // namespace

LcreditTableModel::LcreditTableModel(const DatabaseModelPtr& databaseModel, 
    QObject* parent,
    const OptionalInt& filterId, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)
  , filterId_(filterId)
{
}

LcreditTableModel::~LcreditTableModel()
{
}    

int LcreditTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant LcreditTableModel::headerData(int section, 
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
      if (1 == section || 3 == section)
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

QVariant LcreditTableModel::data(const QModelIndex& index, int role) const
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
    else if (4 == column || 5 == column)
    {        
      QVariant var = QSqlQueryModel::data(index, role);
      if (var.canConvert<QDate>())
      {
        return databaseModel()->convertFromServer(var.value<QDate>());
      }
      else 
      {
        return QVariant(QVariant::Date);
      }
    }
  }
  else if (Qt::TextAlignmentRole == role && index.isValid())
  {
    if (1 == column || 3 == column)
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

TableDescription LcreditTableModel::tableDescription()
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
    
QString LcreditTableModel::internalFilterSql() const
{
  if (filterId_)
  {
    return staticFilters[filterId_.get()];
  }
  return QString();      
}

int LcreditTableModel::internalFilterParamCount() const
{
  if (filterId_)
  {
    return staticFilterParamCount[filterId_.get()];
  }
  return 0;      
}

void LcreditTableModel::bindInternalFilterQueryParams(QSqlQuery& query, 
    std::size_t baseParamNo) const
{
  if (filterId_)
  {
    QDate currentDate = QDate::currentDate();
    QDate tomorrowDate = currentDate.addDays(1);
    QDate thisWeekStartDate = DateUtility::getWeeekStart(currentDate);
    QDate thisWeekEndDate = DateUtility::getWeeekEnd(currentDate);
    QDate thisMonthStartDate = DateUtility::getMonthStart(currentDate);
    QDate thisMonthEndDate = DateUtility::getMonthEnd(currentDate);
    QDate next10DaysDate = currentDate.addDays(10);

    QDate serverCurrentDate = databaseModel()->convertToServer(currentDate);
    QDate serverTomorrowDate = databaseModel()->convertToServer(tomorrowDate);
    QDate serverThisWeekStartDate = 
        databaseModel()->convertToServer(thisWeekStartDate);
    QDate serverThisWeekEndDate = 
        databaseModel()->convertToServer(thisWeekEndDate);
    QDate serverThisMonthStartDate = 
        databaseModel()->convertToServer(thisMonthStartDate);
    QDate serverThisMonthEndDate = 
        databaseModel()->convertToServer(thisMonthEndDate);
    QDate serverNext10DaysDate = 
        databaseModel()->convertToServer(next10DaysDate);
    switch (filterId_.get())
    {              
    case expiredFilterId:
      query.bindValue(boost::numeric_cast<int>(baseParamNo), serverCurrentDate);
      break;
    case expireTodayFilterId:
      query.bindValue(boost::numeric_cast<int>(baseParamNo), serverCurrentDate);
      break;
    case expireTomorrowFilterId:                
      query.bindValue(boost::numeric_cast<int>(baseParamNo), 
          serverTomorrowDate);
      break;
    case expireThisWeekFilterId:
      query.bindValue(boost::numeric_cast<int>(baseParamNo), serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 1), 
          serverThisWeekEndDate);
      break;                
    case expireThisMonthFilterId:
      query.bindValue(boost::numeric_cast<int>(baseParamNo), serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 1),
          serverThisMonthEndDate);
      break;                
    case expireIn10DaysFilterId:
      query.bindValue(boost::numeric_cast<int>(baseParamNo), serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 1), 
          serverNext10DaysDate);
      break;                
    }
  }   
}

QString LcreditTableModel::selectSql() const
{
  return selectPartSql;
}        

} // namespace chrono
} // namespace ma

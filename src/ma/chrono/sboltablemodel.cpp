/*
TRANSLATOR ma::chrono::SbolTableModel
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
#include <ma/chrono/sboltablemodel.h>

namespace ma {

namespace chrono {

namespace {

const int queryColumnCount = 13;
const char* selectPartSql = 
  "select t.\"SBOL_ID\", " \
  "cr.\"NUMBER\", cr.\"PASSPORT_NUMBER\", " \
  "cntp.\"SHORT_NAME\", r.\"SHORT_NAME\", " \
  "t.\"NUMBER\", t.\"SBOL_DATE\", ccd.\"NUMBER\", l.\"NUMBER\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
  "from \"SBOL\" t " \
  "join \"CCD\" ccd on t.\"CCD_ID\" = ccd.\"CCD_ID\" " \
  "join \"CONTRACT\" cr on ccd.\"CONTRACT_ID\" = cr.\"CONTRACT_ID\" " \
  "join \"COUNTERPART\" cntp on cr.\"COUNTERPART_ID\" = cntp.\"COUNTERPART_ID\" " \
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
  "join \"RESIDENT\" r on cr.\"RESIDENT_ID\" = r.\"RESIDENT_ID\" " \
  "left join \"LCREDIT\" l on t.\"LCREDIT_ID\" = l.\"LCREDIT_ID\"";

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Contract number"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Transaction passport"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Counterpart"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Resident"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Shipped bill of lading number"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Date of shipped bill of lading"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "CCD number"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Number of letter of credit"),        
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "User, last update"),        
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Time, last update")        
};
      
const char* staticFilters[SbolTableModel::filterIdCount] = 
{      
  "t.\"SBOL_DATE\" = ?",      
  "t.\"SBOL_DATE\" = ?",            
  "t.\"SBOL_DATE\" >= ? and t.\"SBOL_DATE\" <= ?",
  "t.\"SBOL_DATE\" >= ? and t.\"SBOL_DATE\" <= ?",
  "t.\"SBOL_DATE\" >= ? and t.\"SBOL_DATE\" <= ?",
  "t.\"SBOL_DATE\" >= ? and t.\"SBOL_DATE\" <= ?"
};

const int staticFilterParamCount[SbolTableModel::filterIdCount] = 
{      
  1,
  1,            
  2,
  2,
  2,
  2
};

const int selectedFieldCount = 13;

const char* selectedFieldNames[selectedFieldCount] = 
{
  "cr.\"NUMBER\"",
  "cr.\"PASSPORT_NUMBER\"",
  "t.\"NUMBER\"",
  "cr.\"CONTRACT_ID\"",
  "cntp.\"COUNTERPART_ID\"",
  "r.\"RESIDENT_ID\"",
  "ccd.\"CCD_ID\"",
  "l.\"LCREDIT_ID\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"SBOL_DATE\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Contract number"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Transaction passport"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Shipped bill of lading number"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Contract"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Counterpart"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Resident"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "CCD"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Letter of credit"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Date of shipped bill of lading"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::SbolTableModel", "Time, last update")        
};

const bool selectedFieldNullability[selectedFieldCount] = 
{
  false,
  true,
  false,
  false,
  false,
  false,
  false,
  true,
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
  FieldDescription::entityField,
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
  contractEntity,
  counterpartEntity,
  residentEntity,
  ccdEntity,
  lcreditEntity,
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity
};

} // namespace

SbolTableModel::SbolTableModel(const DatabaseModelPtr& databaseModel, 
    QObject* parent,
    const OptionalInt& filterId, 
    const FilterExpression& filterExpression,
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)
  , filterId_(filterId)
{
}

SbolTableModel::~SbolTableModel()
{
}    

int SbolTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant SbolTableModel::headerData(int section, 
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
      if (1 == section || 2 == section || 5 == section 
          || 7 == section || 8 == section)
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

QVariant SbolTableModel::data(const QModelIndex& index, int role) const
{
  int column = index.column();
  if (Qt::DisplayRole == role)
  {        
    if (11 == column || 12 == column)
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
    else if (6 == column)
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
    if (1 == column || 2 == column || 5 == column || 7 == column || 8 == column)
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
    
TableDescription SbolTableModel::tableDescription()
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

QString SbolTableModel::internalFilterSql() const
{
  if (filterId_)
  {
    return staticFilters[filterId_.get()];
  }
  return QString();      
}

int SbolTableModel::internalFilterParamCount() const
{
  if (filterId_)
  {
    return staticFilterParamCount[filterId_.get()];
  }
  return 0;      
}

void SbolTableModel::bindInternalFilterQueryParams(QSqlQuery& query, 
    std::size_t baseParamNo) const
{
  if (filterId_)
  {
    QDate currentDate = QDate::currentDate();
    QDate yesterdayDate = currentDate.addDays(-1);

    QDate thisWeekStartDate = DateUtility::getWeeekStart(currentDate);
    QDate thisWeekEndDate = DateUtility::getWeeekEnd(currentDate);
    QDate thisMonthStartDate = DateUtility::getMonthStart(currentDate);
    QDate thisMonthEndDate = DateUtility::getMonthEnd(currentDate);             

    QDate prevWeekDay = currentDate.addDays(-7);
    QDate prevWeekStartDate = DateUtility::getWeeekStart(prevWeekDay);
    QDate prevWeekEndDate = DateUtility::getWeeekEnd(prevWeekDay);

    QDate prevMonthDay = currentDate.addMonths(-1);
    QDate prevMonthStartDate = DateUtility::getMonthStart(prevMonthDay);
    QDate prevMonthEndDate = DateUtility::getMonthEnd(prevMonthDay);             

    QDate serverCurrentDate = databaseModel()->convertToServer(currentDate);
    QDate serverYesterdayDate = databaseModel()->convertToServer(yesterdayDate);
    QDate serverThisWeekStartDate = 
        databaseModel()->convertToServer(thisWeekStartDate);
    QDate serverThisWeekEndDate = 
        databaseModel()->convertToServer(thisWeekEndDate);
    QDate serverThisMonthStartDate = 
        databaseModel()->convertToServer(thisMonthStartDate);
    QDate serverThisMonthEndDate = 
        databaseModel()->convertToServer(thisMonthEndDate);
    QDate serverPrevWeekStartDate = 
        databaseModel()->convertToServer(prevWeekStartDate);
    QDate serverPrevWeekEndDate = 
        databaseModel()->convertToServer(prevWeekEndDate);
    QDate serverPrevMonthStartDate = 
        databaseModel()->convertToServer(prevMonthStartDate);
    QDate serverPrevMonthEndDate = 
        databaseModel()->convertToServer(prevMonthEndDate);
    switch (filterId_.get())
    {              
    case todayFilterId:
      query.bindValue(boost::numeric_cast<int>(baseParamNo), serverCurrentDate);
      break;
    case yesterdayFilterId:
      query.bindValue(boost::numeric_cast<int>(baseParamNo), 
          serverYesterdayDate);
      break;
    case thisWeekFilterId:                
      query.bindValue(boost::numeric_cast<int>(baseParamNo), 
          serverThisWeekStartDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 1), 
          serverThisWeekEndDate);
      break;                
    case prevWeekFilterId:
      query.bindValue(boost::numeric_cast<int>(baseParamNo), 
          serverPrevWeekStartDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 1), 
          serverPrevWeekEndDate);
      break;                
    case thisMonthFilterId:
      query.bindValue(boost::numeric_cast<int>(baseParamNo), 
          serverThisMonthStartDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 1),
          serverThisMonthEndDate);
      break;
    case prevMonthFilterId:
      query.bindValue(boost::numeric_cast<int>(baseParamNo), 
          serverPrevMonthStartDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 1),
          serverPrevMonthEndDate);
      break;
    }
  } 
}

QString SbolTableModel::selectSql() const
{
  return selectPartSql;
}       

} // namespace chrono
} // namespace ma

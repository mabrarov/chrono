/*
TRANSLATOR ma::chrono::InsuranceTableModel
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
#include <ma/chrono/insurancetablemodel.h>

namespace ma {

namespace chrono {

namespace {
const int queryColumnCount = 13;
const char* selectPartSql = 
  "select t.\"INSURANCE_ID\", t.\"POLICY_NUMBER\", " \
  "cr.\"NUMBER\", cr.\"CLOSE_MARK\", t.\"AMOUNT\", cur.\"ALPHABETIC_CODE\", " \
  "t.\"SIGNING_DATE\", t.\"START_DATE\", t.\"END_DATE\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
  "from \"INSURANCE\" t " \
  "join \"CONTRACT\" cr on t.\"CONTRACT_ID\" = cr.\"CONTRACT_ID\" " \
  "join \"CURRENCY\" cur on t.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\"";

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "Policy number"),
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "Contract number"),
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "Contract close mark"),
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "Amount"),
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "Currency"),
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "Signing date"),
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "Start date"),
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "End date"),        
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "User, last update"),        
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "Time, last update")        
};
      
const char* staticFilters[InsuranceTableModel::filterIdCount] = 
{      
  "cr.\"CLOSE_MARK\" = 0 and t.\"END_DATE\" < ?",      
  "cr.\"CLOSE_MARK\" = 0 and t.\"END_DATE\" = ?",
  "cr.\"CLOSE_MARK\" = 0 and t.\"END_DATE\" = ?",
  "cr.\"CLOSE_MARK\" = 0 and t.\"END_DATE\" >= ? and t.\"END_DATE\" <= ?",
  "cr.\"CLOSE_MARK\" = 0 and t.\"END_DATE\" >= ? and t.\"END_DATE\" <= ?",
  "cr.\"CLOSE_MARK\" = 0 and t.\"END_DATE\" >= ? and t.\"END_DATE\" <= ?",
};

const int staticFilterParamCount[InsuranceTableModel::filterIdCount] = 
{      
  1,
  1,            
  1,
  2,
  2,
  2
};

const int selectedFieldCount = 10;

const char* selectedFieldNames[selectedFieldCount] = 
{
  "t.\"POLICY_NUMBER\"",
  "cr.\"NUMBER\"",
  "cr.\"CONTRACT_ID\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"SIGNING_DATE\"",
  "t.\"START_DATE\""
  "t.\"END_DATE\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "Policy number"),
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "Contract number"),
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "Contract"),            
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "Signing date"),
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "Start date"),
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "End date"),              
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::InsuranceTableModel", "Time, last update")
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
  false,      
  false
};

const FieldDescription::FieldType selectedFieldTypes[selectedFieldCount] = 
{      
  FieldDescription::stringField,
  FieldDescription::stringField,
  FieldDescription::entityField,
  FieldDescription::entityField,
  FieldDescription::entityField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField
};

const Entity selectedFieldEntitySubTypes[selectedFieldCount] = 
{
  unknownEntity,
  unknownEntity,
  contractEntity,            
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity
};

} // namespace

InsuranceTableModel::InsuranceTableModel(const DatabaseModelPtr& databaseModel,
    QObject* parent,
    const OptionalInt& filterId, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)
  , filterId_(filterId)
{
}

InsuranceTableModel::~InsuranceTableModel()
{
}    

int InsuranceTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant InsuranceTableModel::headerData(int section, 
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
      if (1 == section || 2 == section || 4 == section)
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

QVariant InsuranceTableModel::data(const QModelIndex& index, int role) const
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
    else if (6 == column || 7 == column || 8 == column)
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
    else if (4 == column)
    {
      QVariant amountVar = QSqlQueryModel::data(index, role);          
      if (amountVar.canConvert<qint64>())                    
      {                        
        return QVariant::fromValue(
            model::CurrencyAmount(amountVar.toLongLong()));
      }
      else 
      {
        return QVariant();
      }
    }
    else if (3 == column)
    {
      return QVariant();
    }
  }
  else if (Qt::TextAlignmentRole == role && index.isValid())
  {
    if (1 == column || 2 == column || 4 == column)
    {
      return QVariant(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
    }          
    else if (column)
    {
      return QVariant(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
    }
  } 
  else if (Qt::CheckStateRole == role && 3 == column)
  {
    return QSqlQueryModel::data(index).toInt() != 0 
        ? Qt::Checked : Qt::Unchecked;
  }
  return QSqlQueryModel::data(index, role);      
}
    
TableDescription InsuranceTableModel::tableDescription()
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

QString InsuranceTableModel::internalFilterSql() const
{
  if (filterId_)
  {
    return staticFilters[filterId_.get()];
  }
  return QString();      
}

int InsuranceTableModel::internalFilterParamCount() const
{
  if (filterId_)
  {
    return staticFilterParamCount[filterId_.get()];
  }
  return 0;      
}

void InsuranceTableModel::bindInternalFilterQueryParams(QSqlQuery& query, 
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

QString InsuranceTableModel::selectSql() const
{
  return selectPartSql;
}
    
} // namespace chrono
} // namespace ma

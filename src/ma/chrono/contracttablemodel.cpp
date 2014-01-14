/*
TRANSLATOR ma::chrono::ContractTableModel
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
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
#include <ma/chrono/contracttablemodel.h>

namespace ma {

namespace chrono {

namespace {

const int queryColumnCount = 20;
      
const char* selectPartSql = "select " \
  "t.\"CONTRACT_ID\", t.\"NUMBER\", t.\"PASSPORT_NUMBER\", t.\"SIGNING_DATE\", t.\"END_DATE\", " \
  "t.\"PASSPORT_SIGNING_DATE\", t.\"PASSPORT_END_DATE\", " \
  "t.\"AMOUNT\", cur.\"ALPHABETIC_CODE\", t.\"CLOSE_MARK\", " \
  "t.\"CLOSE_DATE\", " \
  "cntpr.\"SHORT_NAME\", b.\"SHORT_NAME\", r.\"SHORT_NAME\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", clu.\"LOGIN\", " \
  "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", t.\"CLOSE_TIME\" " \
  "from \"CONTRACT\" t " \
  "join \"CURRENCY\" cur on t.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
  "left join \"USER\" clu on t.\"CLOSE_USER_ID\" = clu.\"USER_ID\" " \
  "join \"COUNTERPART\" cntpr on t.\"COUNTERPART_ID\" = cntpr.\"COUNTERPART_ID\" " \
  "join \"BANK\" b on t.\"BANK_ID\" = b.\"BANK_ID\" " \
  "join \"RESIDENT\" r on t.\"RESIDENT_ID\" = r.\"RESIDENT_ID\"";

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Contract number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Passport number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Contract signing date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Contract end date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Passport signing date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Passport end date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Amount"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Currency"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Close mark"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Close date"),                        
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Counterpart"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Bank"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Resident"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "User, closed"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Time, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Time, closed")
};
      
const char* staticFilters[ContractTableModel::filterIdCount] = 
{
  "t.\"CLOSE_MARK\" != 0",
  "t.\"CLOSE_MARK\" = 0 and (t.\"END_DATE\" < ? or t.\"PASSPORT_END_DATE\" < ?)",
  "t.\"CLOSE_MARK\" = 0",
  "t.\"CLOSE_MARK\" = 0 and t.\"END_DATE\" >= ? and t.\"PASSPORT_END_DATE\" >= ? and (t.\"END_DATE\" = ? or t.\"PASSPORT_END_DATE\" = ?)",
  "t.\"CLOSE_MARK\" = 0 and t.\"END_DATE\" >= ? and t.\"PASSPORT_END_DATE\" >= ? and (t.\"END_DATE\" = ? or t.\"PASSPORT_END_DATE\" = ?)",
  "t.\"CLOSE_MARK\" = 0 and t.\"END_DATE\" >= ? and t.\"PASSPORT_END_DATE\" >= ? and ((t.\"END_DATE\" >= ? and t.\"END_DATE\" <= ?) or (t.\"PASSPORT_END_DATE\" >= ? and t.\"PASSPORT_END_DATE\" <= ?))",
  "t.\"CLOSE_MARK\" = 0 and t.\"END_DATE\" >= ? and t.\"PASSPORT_END_DATE\" >= ? and ((t.\"END_DATE\" >= ? and t.\"END_DATE\" <= ?) or (t.\"PASSPORT_END_DATE\" >= ? and t.\"PASSPORT_END_DATE\" <= ?))",
  "t.\"CLOSE_MARK\" = 0 and t.\"END_DATE\" >= ? and t.\"PASSPORT_END_DATE\" >= ? and ((t.\"END_DATE\" >= ? and t.\"END_DATE\" <= ?) or (t.\"PASSPORT_END_DATE\" >= ? and t.\"PASSPORT_END_DATE\" <= ?))",
};

const int staticFilterParamCount[ContractTableModel::filterIdCount] = 
{      
  0,
  2,            
  0,
  4,
  4,
  6,
  6,
  6
};

const int selectedFieldCount = 21;

const char* selectedFieldNames[selectedFieldCount] = 
{
  "t.\"NUMBER\"",
  "t.\"PASSPORT_NUMBER\"",
  "cur.\"CURRENCY_ID\"",      
  "cntpr.\"COUNTERPART_ID\"",
  "b.\"BANK_ID\"",
  "r.\"RESIDENT_ID\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "clu.\"USER_ID\"",
  "t.\"SIGNING_DATE\"",
  "t.\"END_DATE\"",
  "t.\"PASSPORT_SIGNING_DATE\"",
  "t.\"PASSPORT_END_DATE\"",
  "t.\"CLOSE_DATE\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\"",
  "t.\"CLOSE_TIME\"",
  "\"CCD_ID\"",
  "\"PAY_SHEET_ID\"",
  "\"INSURANCE_ID\"",
  "\"LCREDIT_ID\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Contract number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Passport number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Currency"),      
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Counterpart"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Bank"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Resident"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "User, closed"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Contract signing date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Contract end date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Passport signing date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Passport end date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Close date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Time, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Time, closed"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "List of CCD"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "List of Pay Sheets"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "Insurance list"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractTableModel", "List of Letters of Credit")
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
  false,
  false,
  false,
  false,
  false,
  false,
  true,
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
  FieldDescription::entityField,
  FieldDescription::entityField,
  FieldDescription::entityField,
  FieldDescription::entityField,
  FieldDescription::entityField,
  FieldDescription::entityField,
  FieldDescription::entityField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::entitySetField,
  FieldDescription::entitySetField,
  FieldDescription::entitySetField,
  FieldDescription::entitySetField
};

const Entity selectedFieldEntitySubTypes[selectedFieldCount] = 
{
  unknownEntity,
  unknownEntity,
  currencyEntity,      
  counterpartEntity,
  bankEntity,
  residentEntity,
  userEntity,
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  ccdEntity,
  paysheetEntity,
  insuranceEntity,
  lcreditEntity
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
  0,
  0,
  0,
  0,
  0,
  0,
  0,        
  "t.\"CONTRACT_ID\"",
  "t.\"CONTRACT_ID\"",
  "t.\"CONTRACT_ID\"",
  "t.\"CONTRACT_ID\""
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
  0,
  0,
  0,
  0,
  0,
  0,
  0,     
  "\"CCD\"",
  "\"PAY_SHEET\"",
  "\"INSURANCE\"",
  "\"LCREDIT\""
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
  0,
  0,
  0,
  0,
  0,
  0,
  0,     
  "\"CONTRACT_ID\"",
  "\"CONTRACT_ID\"",
  "\"CONTRACT_ID\"",
  "\"CONTRACT_ID\""
};

} // namespace

ContractTableModel::ContractTableModel(const DatabaseModelPtr& databaseModel, 
    QObject* parent,
    const OptionalInt& filterId, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)
  , filterId_(filterId)
{
}

ContractTableModel::~ContractTableModel()
{
}    

int ContractTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant ContractTableModel::headerData(int section, 
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
      if (1 == section || 2 == section || 7 == section)
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

QVariant ContractTableModel::data(const QModelIndex& index, int role) const
{
  int column = index.column();
  if (Qt::DisplayRole == role)
  {        
    if (17 == column || 18 == column || 19 == column)
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
    else if (3 == column || 4 == column || 5 == column || 6 == column)
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
    else if (7 == column)
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
    else if (10 == column)
    {
      QVariant var = QSqlQueryModel::data(index, role);
      if (var.isNull())
      {
        return var;
      }
      if (var.canConvert<QDate>())
      {
        return databaseModel()->convertFromServer(var.value<QDate>());
      }
      else 
      {
        return QVariant(QVariant::Date);
      }
    } 
    else if (9 == column)
    {
      return QVariant();
    }
  }
  else if (Qt::TextAlignmentRole == role && index.isValid())
  {
    if (1 == column || 2 == column || 7 == column)
    {
      return QVariant(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
    }          
    else if (column)
    {
      return QVariant(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
    }
  }
  else if (Qt::CheckStateRole == role && 9 == column)
  {
    return QSqlQueryModel::data(index).toInt() != 0 
        ? Qt::Checked : Qt::Unchecked;
  }
  return QSqlQueryModel::data(index, role);      
}    

Qt::ItemFlags ContractTableModel::flags(const QModelIndex& index) const
{
  if (7 == index.column())
  {
    return QSqlQueryModel::flags(index) | Qt::ItemIsUserCheckable;
  }
  return QSqlQueryModel::flags(index);
}

TableDescription ContractTableModel::tableDescription()
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
              ? QString(selectedFieldEntitySetRelationFieldName[i]) 
                  : QString()));
    }           
  }      
  return tableDescription;
}

QString ContractTableModel::internalFilterSql() const
{
  if (filterId_)
  {
    return staticFilters[filterId_.get()];
  }
  return QString();      
}

int ContractTableModel::internalFilterParamCount() const
{
  if (filterId_)
  {
    return staticFilterParamCount[filterId_.get()];
  }
  return 0;      
}

void ContractTableModel::bindInternalFilterQueryParams(QSqlQuery& query,
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
      query.bindValue(boost::numeric_cast<int>(baseParamNo),
          serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 1),
          serverCurrentDate);
      break;
    case expireTodayFilterId:
      query.bindValue(boost::numeric_cast<int>(baseParamNo),
          serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 1),
          serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 2),
          serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 3),
          serverCurrentDate);
      break;
    case expireTomorrowFilterId:                
      query.bindValue(boost::numeric_cast<int>(baseParamNo), 
          serverTomorrowDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 1),
          serverTomorrowDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 2), 
          serverTomorrowDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 3), 
          serverTomorrowDate);
      break;
    case expireThisWeekFilterId:
      query.bindValue(boost::numeric_cast<int>(baseParamNo), 
          serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 1),
          serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 2), 
          serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 3), 
          serverThisWeekEndDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 4), 
          serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 5), 
          serverThisWeekEndDate);
      break;                
    case expireThisMonthFilterId:
      query.bindValue(boost::numeric_cast<int>(baseParamNo), 
          serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 1),
          serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 2), 
          serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 3),
          serverThisMonthEndDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 4),
          serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 5),
          serverThisMonthEndDate);
      break;                
    case expireIn10DaysFilterId:
      query.bindValue(boost::numeric_cast<int>(baseParamNo), 
          serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 1), 
          serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 2), 
          serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 3), 
          serverNext10DaysDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 4), 
          serverCurrentDate);
      query.bindValue(boost::numeric_cast<int>(baseParamNo + 5),
          serverNext10DaysDate);
      break;                
    }
  } 
}

QString ContractTableModel::selectSql() const
{
  return selectPartSql;
}       

} // namespace chrono
} // namespace ma

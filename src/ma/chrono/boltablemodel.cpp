/*
TRANSLATOR ma::chrono::BolTableModel
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
#include <ma/chrono/boltablemodel.h>

namespace ma {

namespace chrono {

namespace {    

const int queryColumnCount = 17;
const char* selectPartSql = 
  "select t.\"BOL_ID\", t.\"NUMBER\", t.\"BOL_DATE\", ccd.\"NUMBER\", " \
  "cr.\"NUMBER\", cr.\"PASSPORT_NUMBER\", " \
  "cntp.\"SHORT_NAME\", " \
  "a.\"SHORT_NAME\", t.\"VEHICLE_NUMBER\", " \
  "t.\"PLACE_COUNT\", t.\"CARGO_WEIGHT\", t.\"CARGO_VOLUME\", t.\"LOAD_PERM_DATE\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
  "from \"BOL\" t " \
  "join \"CCD\" ccd on t.\"CCD_ID\" = ccd.\"CCD_ID\" " \
  "join \"CONTRACT\" cr on ccd.\"CONTRACT_ID\" = cr.\"CONTRACT_ID\" " \
  "join \"COUNTERPART\" cntp on cr.\"COUNTERPART_ID\" = cntp.\"COUNTERPART_ID\" " \
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
  "join \"AUTOENTERPRISE\" a on t.\"AUTOENTERPRISE_ID\" = a.\"AUTOENTERPRISE_ID\"";

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "BOL number"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "BOL date"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "CCD number"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Contract number"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Transaction passport number"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Counterpart"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Autoenterprise"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Vehicle number"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Place count"), 
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Cargo weight"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Cargo volume"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Load permission date"),        
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "User, last update"),        
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Time, last update")        
};
      
const char* staticFilters[BolTableModel::filterIdCount] = 
{      
  "t.\"BOL_DATE\" = ?",      
  "t.\"BOL_DATE\" = ?",            
  "t.\"BOL_DATE\" >= ? and t.\"BOL_DATE\" <= ?",
  "t.\"BOL_DATE\" >= ? and t.\"BOL_DATE\" <= ?",
  "t.\"BOL_DATE\" >= ? and t.\"BOL_DATE\" <= ?",
  "t.\"BOL_DATE\" >= ? and t.\"BOL_DATE\" <= ?"
};

const int staticFilterParamCount[BolTableModel::filterIdCount] = 
{      
  1,
  1,            
  2,
  2,
  2,
  2
};

const int selectedFieldCount = 12;

const char* selectedFieldNames[selectedFieldCount] = 
{
  "t.\"NUMBER\"", 
  "ccd.\"NUMBER\"",
  "ccd.\"CCD_ID\"",
  "cr.\"CONTRACT_ID\"",
  "cntp.\"COUNTERPART_ID\"",
  "a.\"AUTOENTERPRISE_ID\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"BOL_DATE\"",
  "t.\"LOAD_PERM_DATE\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "BOL number"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "CCD number"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "CCD"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Contract"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Counterpart"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Autoenterprise"),      
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "BOL date"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Load permission date"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::BolTableModel", "Time, last update")
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
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField,
  FieldDescription::dateField
};

const Entity selectedFieldEntitySubTypes[selectedFieldCount] = 
{
  unknownEntity,
  unknownEntity,
  ccdEntity,
  contractEntity,
  counterpartEntity,
  autoenterpriseEntity,
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity
};

} // namespace 

BolTableModel::BolTableModel(const DatabaseModelPtr& databaseModel, 
    QObject* parent,
    const OptionalInt& filterId, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)
  , filterId_(filterId)
{
}

BolTableModel::~BolTableModel()
{
}    

int BolTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant BolTableModel::headerData(int section, 
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
      if (1 == section || 5 == section || 3 == section || 4 == section 
          || 8 == section || 9 == section || 10 == section || 11 == section)
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

QVariant BolTableModel::data(const QModelIndex& index, int role) const
{
  int column = index.column();
  if (Qt::DisplayRole == role)
  {        
    if (16 == column || 15 == column)
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
    else if (2 == column || 12 == column)
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
    if (1 == column || 5 == column || 3 == column || 4 == column 
        || 8 == column || 9 == column || 10 == column || 11 == column)
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

TableDescription BolTableModel::tableDescription()
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

QString BolTableModel::internalFilterSql() const
{
  if (filterId_)
  {
    return staticFilters[filterId_.get()];
  }
  return QString();      
}

int BolTableModel::internalFilterParamCount() const
{
  if (filterId_)
  {
    return staticFilterParamCount[filterId_.get()];
  }
  return 0;      
}

void BolTableModel::bindInternalFilterQueryParams(QSqlQuery& query, 
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

QString BolTableModel::selectSql() const
{
  return selectPartSql;
}       

} // namespace chrono
} //namespace ma

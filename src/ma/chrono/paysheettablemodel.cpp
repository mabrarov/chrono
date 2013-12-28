/*
TRANSLATOR ma::chrono::PaysheetTableModel
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
#include <ma/chrono/paysheettablemodel.h>

namespace ma {

namespace chrono {

namespace {    

const int queryColumnCount = 13;

const char* selectPartSql = 
  "select t.\"PAY_SHEET_ID\", " \
  "cr.\"NUMBER\", cr.\"PASSPORT_NUMBER\", c.\"NUMBER\", " \
  "t.\"DOC_NUMBER\", t.\"PAY_DATE\", t.\"AMOUNT\", " \
  "cur.\"ALPHABETIC_CODE\", t.\"ACCOUNT_NUMBER\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
  "from \"PAY_SHEET\" t " \
  "join \"CONTRACT\" cr on t.\"CONTRACT_ID\" = cr.\"CONTRACT_ID\" " \
  "left join \"CCD\" c on t.\"CCD_ID\" = c.\"CCD_ID\" " \
  "join \"CURRENCY\" cur on t.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\"";

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "No."),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Contract number"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Passport number"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "CCD number"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Document number"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Pay date"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Amount"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Currency"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Account number"),        
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "User, last update"),        
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Time, last update")        
};

const char* staticFilters[PaysheetTableModel::filterIdCount] = 
{
  "exists (select 1 from \"PAYSHEET_ICT_RELATION\" where \"PAYSHEET_ICT_RELATION\".\"PAY_SHEET_ID\" = t.\"PAY_SHEET_ID\")",
  "not exists (select 1 from \"PAYSHEET_ICT_RELATION\" where \"PAYSHEET_ICT_RELATION\".\"PAY_SHEET_ID\" = t.\"PAY_SHEET_ID\")"        
};

const int staticFilterParamCount[PaysheetTableModel::filterIdCount] = 
{      
  0,
  0
};

const int selectedFieldCount = 13;

const char* selectedFieldNames[selectedFieldCount] = 
{
  "cr.\"NUMBER\"",
  "cr.\"PASSPORT_NUMBER\"",
  "c.\"NUMBER\"",
  "t.\"DOC_NUMBER\"",
  "t.\"ACCOUNT_NUMBER\"",
  "cr.\"CONTRACT_ID\"",
  "c.\"CCD_ID\"",
  "cur.\"CURRENCY_ID\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"PAY_DATE\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Contract number"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Passport number"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "CCD number"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Document number"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Account number"),        
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Contract"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "CCD"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Currency"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Pay date"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::PaysheetTableModel", "Time, last update")
};

const bool selectedFieldNullability[selectedFieldCount] = 
{
  false,
  true,
  true,
  false,
  true,
  false,
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
  unknownEntity,
  unknownEntity,
  contractEntity,
  ccdEntity,
  currencyEntity,
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity
};

} // namespace

PaysheetTableModel::PaysheetTableModel(const DatabaseModelPtr& databaseModel,
    QObject* parent,
    const OptionalInt& filterId, 
    const FilterExpression& filterExpression,
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)
  , filterId_(filterId)
{
}

PaysheetTableModel::~PaysheetTableModel()
{
}    

int PaysheetTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant PaysheetTableModel::headerData(int section, 
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
      if (1 == section || 2 == section || 3 == section || 4 == section 
          || 6 == section || 8 == section)
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

QVariant PaysheetTableModel::data(const QModelIndex& index, int role) const
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
    else if (5 == column)
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
    else if (6 == column)
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
  }
  else if (Qt::TextAlignmentRole == role && index.isValid())
  {
    if (1 == column || 2 == column || 3 == column || 4 == column 
        || 6 == column || 8 == column)
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
    
TableDescription PaysheetTableModel::tableDescription()
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

QString PaysheetTableModel::internalFilterSql() const
{
  if (filterId_)
  {
    return staticFilters[filterId_.get()];
  }
  return QString();      
}

int PaysheetTableModel::internalFilterParamCount() const
{
  if (filterId_)
  {
    return staticFilterParamCount[filterId_.get()];
  }
  return 0;      
}

void PaysheetTableModel::bindInternalFilterQueryParams(QSqlQuery& /*query*/, 
    std::size_t /*baseParamNo*/) const
{
  if (filterId_)
  {
    //todo      
  } 
}

QString PaysheetTableModel::selectSql() const
{
  return selectPartSql;
}       

} // namespace chrono
} //namespace ma

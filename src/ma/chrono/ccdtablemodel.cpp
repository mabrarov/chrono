/*
TRANSLATOR ma::chrono::CcdTableModel
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <boost/make_shared.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/model/currencyamount.h>
#include <ma/chrono/model/rateamount.h>
#include <ma/chrono/ccdtablemodel.h>

namespace ma {

namespace chrono {

namespace {

const int queryColumnCount = 22;
const char* selectPartSql = 
  "select t.\"CCD_ID\", t.\"NUMBER\", t.\"AMOUNT\", " \
  "cur.\"ALPHABETIC_CODE\", cr.\"NUMBER\", cr.\"PASSPORT_NUMBER\", " \
  "cntp.\"SHORT_NAME\", " \
  "d.\"SHORT_NAME\", b.\"SHORT_NAME\", " \
  "t.\"EXPORT_PERM_DATE\", tt.\"NAME\", t.\"DUTY\", t.\"RATE\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", " \
  "t.\"PAYMENT_LIMIT_DATE\", t.\"ZERO_TAX_LIMIT_DATE\", t.\"EXPORT_DATE\", " \
  "t.\"PAYMENT_CONTROL_FLAG\", t.\"ZERO_TAX_CONTROL_FLAG\" " \
  "from \"CCD_SEL\" t " \
  "join \"CONTRACT\" cr on t.\"CONTRACT_ID\" = cr.\"CONTRACT_ID\" " \
  "join \"COUNTERPART\" cntp on cr.\"COUNTERPART_ID\" = cntp.\"COUNTERPART_ID\" " \
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
  "join \"DEPPOINT\" d on t.\"DEPPOINT_ID\" = d.\"DEPPOINT_ID\" " \
  "join \"BANK\" b on cr.\"BANK_ID\" = b.\"BANK_ID\" " \
  "join \"TTERM\" tt on t.\"TTERM_ID\" = tt.\"TTERM_ID\" " \
  "join \"CURRENCY\" cur on cr.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\"";

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "CCD number"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Amount"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Currency"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Contract number"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Passport number"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Counterpart"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Deppoint"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Bank"),                        
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Export permission date"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Transaction terms"),        
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Duty"),        
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Rate"),        
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "User, last update"),        
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Time, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Payment limit date"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Zero tax limit date"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Export date"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Payment control flag"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Zero tax control flag")
};

const char* staticFilters[CcdTableModel::filterIdCount] = 
{
  "exists (select 1 from \"CCD_ICDOC_RELATION\" where \"CCD_ICDOC_RELATION\".\"CCD_ID\" = t.\"CCD_ID\")",
  "not exists (select 1 from \"CCD_ICDOC_RELATION\" where \"CCD_ICDOC_RELATION\".\"CCD_ID\" = t.\"CCD_ID\")",
  "t.\"PAYMENT_CONTROL_FLAG\" <> 0 AND t.\"PAYMENT_LIMIT_DATE\" < ?",
  "t.\"ZERO_TAX_CONTROL_FLAG\" <> 0 AND t.\"ZERO_TAX_LIMIT_DATE\" < ?"        
};

const int staticFilterParamCount[CcdTableModel::filterIdCount] = 
{      
  0,
  0,            
  1,
  1
};

const int selectedFieldCount = 23;

const char* selectedFieldNames[selectedFieldCount] = 
{
  "t.\"NUMBER\"",
  "cr.\"NUMBER\"",
  "cr.\"PASSPORT_NUMBER\"",
  "cur.\"CURRENCY_ID\"",
  "cr.\"CONTRACT_ID\"",
  "cntp.\"COUNTERPART_ID\"",
  "d.\"DEPPOINT_ID\"",
  "b.\"BANK_ID\"",
  "tt.\"TTERM_ID\"",
  "cu.\"USER_ID\"",
  "uu.\"USER_ID\"",
  "t.\"EXPORT_PERM_DATE\"",      
  "t.\"EXPORT_DATE\"",
  "t.\"PAYMENT_LIMIT_DATE\"",
  "t.\"ZERO_TAX_LIMIT_DATE\"",
  "t.\"CREATE_TIME\"",
  "t.\"UPDATE_TIME\"",
  "\"ICDOC_ID\"",
  "\"BOL_ID\"",
  "\"SBOL_ID\"",        
  "\"PAY_SHEET_ID\"",
  "t.\"PAYMENT_CONTROL_FLAG\"",
  "t.\"ZERO_TAX_CONTROL_FLAG\""
};

const char* selectedFieldTitles[selectedFieldCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "CCD number"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Contract number"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Passport number"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Currency"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Contract"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Counterpart"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Deppoint"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Bank"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Transaction terms"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Export permission date"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Export date"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Payment limit date"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Zero tax limit date"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Time, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "List of inquiry on confirming documents"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "List of bills of lading"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "List of shipped bills of lading"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "List of Pay Sheets"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Payment control flag"),
  QT_TRANSLATE_NOOP("ma::chrono::CcdTableModel", "Zero tax control flag")
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
  false,
  false,
  true,
  true,
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
  FieldDescription::entityField,
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
  FieldDescription::entitySetField,
  FieldDescription::entitySetField,
  FieldDescription::entitySetField,
  FieldDescription::entitySetField,
  FieldDescription::boolField,
  FieldDescription::boolField
};

const Entity selectedFieldEntitySubTypes[selectedFieldCount] = 
{
  unknownEntity,
  unknownEntity,
  unknownEntity,
  currencyEntity,
  contractEntity,
  counterpartEntity,
  deppointEntity,
  bankEntity,
  ttermEntity,
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  icdocEntity,
  bolEntity,
  sbolEntity,
  paysheetEntity,
  unknownEntity,
  unknownEntity
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
  "t.\"CCD_ID\"",
  "t.\"CCD_ID\"",
  "t.\"CCD_ID\"",
  "t.\"CCD_ID\"",
  0,        
  0
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
  "\"CCD_ICDOC_RELATION\"",      
  "\"BOL\"",
  "\"SBOL\"",
  "\"PAY_SHEET\"",
  0,
  0
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
  "\"CCD_ID\"",
  "\"CCD_ID\"",
  "\"CCD_ID\"",
  "\"CCD_ID\"",
  0,
  0
};

} // namespace

CcdTableModel::CcdTableModel(const DatabaseModelPtr& databaseModel, 
    QObject* parent, 
    const OptionalInt& filterId, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)
  , filterId_(filterId)
{      
}

CcdTableModel::~CcdTableModel()
{
}    

int CcdTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant CcdTableModel::headerData(int section, 
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
      if (1 == section || 2 == section || 4 == section 
          || 5 == section || 11 == section || 12 == section)
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

QVariant CcdTableModel::data(const QModelIndex& index, int role) const
{
  int column = index.column();
  if (Qt::DisplayRole == role)
  {        
    if (15 == column || 16 == column)
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
    else if (9 == column || 17 == column || 18 == column || 19 == column)
    {        
      QVariant var = QSqlQueryModel::data(index, role);
      if (!var.isNull() && var.canConvert<QDate>())
      {
        return databaseModel()->convertFromServer(var.value<QDate>());
      }
      else 
      {
        return QVariant(QVariant::Date);
      }
    }
    else if (2 == column || 11 == column)
    {
      QVariant amountVar = QSqlQueryModel::data(index, role);          
      if (amountVar.canConvert<qint64>())                    
      {               
        return QVariant::fromValue<model::CurrencyAmount>(
            model::CurrencyAmount(amountVar.toLongLong()));
      } 
      else 
      {
        return QVariant();
      }
    }
    else if (12 == column)
    {
      QVariant amountVar = QSqlQueryModel::data(index, role);          
      if (amountVar.canConvert<qint64>())                    
      {               
        return QVariant::fromValue<model::RateAmount>(
            model::RateAmount(amountVar.toLongLong()));
      }
      else 
      {
        return QVariant();
      }
    }
    else if (20 == column || 21 == column)
    {
      return QVariant();
    }
  }
  else if (Qt::TextAlignmentRole == role && index.isValid())
  {
    if (1 == column || 2 == column || 4 == column || 5 == column 
        || 11 == column || 12 == column)
    {
      return QVariant(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
    }          
    else if (column)
    {
      return QVariant(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
    }
  }      
  else if (Qt::CheckStateRole == role && (20 == column || 21 == column))
  {
    return QSqlQueryModel::data(index).toInt() != 0 
        ? Qt::Checked : Qt::Unchecked;
  }
  return QSqlQueryModel::data(index, role);      
}       

TableDescription CcdTableModel::tableDescription()
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

QString CcdTableModel::internalFilterSql() const
{
  if (filterId_)
  {
    return staticFilters[filterId_.get()];
  }
  return QString();      
}

int CcdTableModel::internalFilterParamCount() const
{
  if (filterId_)
  {
    return staticFilterParamCount[filterId_.get()];
  }
  return 0;      
}

void CcdTableModel::bindInternalFilterQueryParams(QSqlQuery& query, 
    std::size_t baseParamNo) const
{
  if (filterId_)
  {
    QDate currentDate = QDate::currentDate();
    QDate serverCurrentDate = databaseModel()->convertToServer(currentDate);
    switch (filterId_.get())
    {
    case paymentControlExpired:
    case zeroTaxControlExpired:
      query.bindValue(boost::numeric_cast<int>(baseParamNo), serverCurrentDate);
      break;
    }
  } 
}

QString CcdTableModel::selectSql() const
{
  return selectPartSql;
}       

} // namespace chrono
} //namespace ma

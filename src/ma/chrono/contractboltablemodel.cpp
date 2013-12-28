/*
TRANSLATOR ma::chrono::ContractBolTableModel
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <boost/make_shared.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/contractboltablemodel.h>

namespace ma {

namespace chrono {

namespace {

const int queryColumnCount = 14;
const char* selectPartSql = 
  "select t.\"BOL_ID\", t.\"NUMBER\", t.\"BOL_DATE\", ccd.\"NUMBER\", " \
  "a.\"SHORT_NAME\", t.\"VEHICLE_NUMBER\", " \
  "t.\"PLACE_COUNT\", t.\"CARGO_WEIGHT\", t.\"CARGO_VOLUME\", t.\"LOAD_PERM_DATE\", " \
  "cu.\"LOGIN\", uu.\"LOGIN\", " \
  "t.\"CREATE_TIME\", t.\"UPDATE_TIME\" " \
  "from \"BOL\" t " \
  "join \"CCD\" ccd on t.\"CCD_ID\" = ccd.\"CCD_ID\" " \
  "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
  "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
  "join \"AUTOENTERPRISE\" a on t.\"AUTOENTERPRISE_ID\" = a.\"AUTOENTERPRISE_ID\"";

const char* columnHeaders[queryColumnCount] = 
{
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "No."),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "BOL number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "BOL date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "CCD number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "Autoenterprise"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "Vehicle number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "Place count"), 
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "Cargo weight"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "Cargo volume"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "Load permission date"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "User, last update"),        
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "Time, last update")        
};
      
const int selectedFieldCount = 10;

const char* selectedFieldNames[selectedFieldCount] = 
{
  "t.\"NUMBER\"", 
  "ccd.\"NUMBER\"",
  "ccd.\"CCD_ID\"",
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
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "BOL number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "CCD number"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "CCD"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "Autoenterprise"),      
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "User, creator"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "User, last update"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "BOL date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "Load permission date"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "Time, created"),
  QT_TRANSLATE_NOOP("ma::chrono::ContractBolTableModel", "Time, last update")
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
  autoenterpriseEntity,
  userEntity,
  userEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity,
  unknownEntity
};

} // namespace 
   
ContractBolTableModel::ContractBolTableModel(
    const DatabaseModelPtr& databaseModel,
    QObject* parent, 
    const OptionalQInt64& contractId, 
    const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : BasicTableModel(databaseModel, parent, filterExpression, orderExpression)
  , contractId_(contractId)
{      
}

ContractBolTableModel::~ContractBolTableModel()
{
}    

int ContractBolTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return queryColumnCount;
}

QVariant ContractBolTableModel::headerData(int section, 
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
      if (1 == section || 3 == section || 4 == section)
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

QVariant ContractBolTableModel::data(const QModelIndex& index, int role) const
{
  int column = index.column();
  if (Qt::DisplayRole == role)
  {        
    if (7 == column || 8 == column)
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
    else if (2 == column)
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
  }
  else if (Qt::TextAlignmentRole == role && index.isValid())
  {
    if (1 == column || 3 == column || 4 == column)
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

TableDescription ContractBolTableModel::tableDescription()
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

void ContractBolTableModel::setContractId(qint64 contractId)
{
  if (!contractId_)
  {
    contractId_ = contractId;
  }
}

QString ContractBolTableModel::internalFilterSql() const
{
  if (contractId_)
  {
    const static QString contractFilterSql("ccd.\"CONTRACT_ID\" = ?");
    return contractFilterSql;
  }
  return QString();
}

int ContractBolTableModel::internalFilterParamCount() const
{
  if (contractId_)
  {
    return 1;
  }
  return 0;
}

void ContractBolTableModel::bindInternalFilterQueryParams(QSqlQuery& query, 
    std::size_t baseParamNo) const
{
  if (contractId_)
  {
    query.bindValue(boost::numeric_cast<int>(baseParamNo), contractId_.get());
  }
}

QString ContractBolTableModel::selectSql() const
{
  return selectPartSql;
}
        
} // namespace chrono
} //namespace ma
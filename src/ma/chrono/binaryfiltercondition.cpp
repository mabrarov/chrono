/*
TRANSLATOR ma::chrono::BinaryFilterCondition
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <stdexcept> 
#include <boost/throw_exception.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <QCoreApplication>
#include <QStringList>
#include <QDate>
#include <QDateTime>
#include <QSqlQuery>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/binaryfiltercondition.h>

namespace ma {
namespace chrono {       

namespace {

const char* translationContext = "ma::chrono::BinaryFilterCondition";

}

BinaryFilterCondition::BinaryFilterCondition(const FieldDescriptionPtr& field, 
    ConditionType conditionType, const QString& rightValueText, 
    const QVariant& rightValue)
  : OneFieldBasedFilterCondition(field)
  , conditionType_(conditionType)
  , rightValueText_(rightValueText)
  , rightValue_(rightValue)
{
  static const char* conditionTypeTextFormat = QT_TR_NOOP("Field \"%1\" %2 %3");
  static const char* conditionTypeText[conditionTypeCount] = 
  {
    QT_TR_NOOP("=="),
    QT_TR_NOOP("!="),
    QT_TR_NOOP(">"),
    QT_TR_NOOP("<"),
    QT_TR_NOOP(">="),
    QT_TR_NOOP("<="),
    QT_TR_NOOP("like"),
    QT_TR_NOOP("not like"),
    QT_TR_NOOP("contains"),
    QT_TR_NOOP("not contains")
  };

  QString conditionTextFormat(
      qApp->translate(translationContext, conditionTypeTextFormat));
  setConditionText(conditionTextFormat
      .arg(field->title())
      .arg(qApp->translate(
          translationContext, conditionTypeText[conditionType_]))
      .arg(rightValueText_));

  static const char* conditionTypeSql[conditionTypeCount] = 
  {
    "%1 = ?",
    "%1 <> ?",
    "%1 > ?",
    "%1 < ?",
    "%1 >= ?",        
    "%1 <= ?",                
    "%1 like ? escape '\\'",
    "%1 not like ? escape '\\'",
    "exists (select 1 from %2 where %3 = %2.%4 and %2.%1 = ?)",
    "not exists (select 1 from %2 where %3 = %2.%4 and %2.%1 = ?)"
  };               

  QString sql(conditionTypeSql[conditionType_]);      
  if (FieldDescription::entitySetField != field->fieldType())
  {
    setSql(sql.arg(field->fieldName()));
  }
  else if (containsCondition == conditionType 
      || notContainsCondition == conditionType)
  {        
    setSql(sql
        .arg(field->fieldName())
        .arg(field->entitySetTableName())
        .arg(field->relationFieldName())
        .arg(field->entitySetRelationFieldName()));
  }
  else 
  {
    boost::throw_exception(
        std::invalid_argument("unsupported entity set condition"));
  }
}

std::size_t BinaryFilterCondition::paramCount() const
{
  return 1;
}    

void BinaryFilterCondition::bindQueryParams(QSqlQuery& query, 
    std::size_t baseParamNo, const DatabaseModel& databaseModel) const
{
  QVariant rightValue(rightValue_);
  QVariant::Type rightValueType = rightValue_.type();
  if (QVariant::Date == rightValueType)
  {
    rightValue = databaseModel.convertFromServer(rightValue_.value<QDate>());
  }
  else if (QVariant::DateTime == rightValueType)
  {
    rightValue = 
        databaseModel.convertFromServer(rightValue_.value<QDateTime>());
  }
  else if (QVariant::String == rightValueType)
  {
    if (likeCondition == conditionType_ || notLikeCondition == conditionType_)
    {
      QString str = rightValue_.value<QString>();          
      str.replace("\\", "\\\\");
      str.replace("%", "\\%");
      str.replace("_", "\\_");
      str.replace("*", "%");
      str.replace("?", "_");
      rightValue = str;          
    }
  }
  query.bindValue(boost::numeric_cast<int>(baseParamNo), rightValue);
}

} // namespace chrono
} //namespace ma

/*
TRANSLATOR ma::chrono::UnaryFilterCondition
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <stdexcept> 
#include <boost/throw_exception.hpp>
#include <QCoreApplication>
#include <QStringList>
#include <ma/chrono/unaryfiltercondition.h>

namespace ma
{
  namespace chrono
  {
    namespace
    {
      const char* translationContext("ma::chrono::UnaryFilterCondition");
    }

    UnaryFilterCondition::UnaryFilterCondition(const FieldDescriptionPtr& field, ConditionType conditionType)
      : OneFieldBasedFilterCondition(field)
      , conditionType_(conditionType)
    {      
      static const char* conditionTypeTextFormat[conditionTypeCount] = 
      {
        QT_TR_NOOP("Field \"%1\" is not defined"),
        QT_TR_NOOP("Field \"%1\" is defined"),
        QT_TR_NOOP("\"%1\" is empty"),
        QT_TR_NOOP("\"%1\" is not empty")
      };

      static const char* conditionTypeSql[conditionTypeCount] = 
      {
        "%1 is null",
        "%1 is not null",
        "not exists (select 1 from %1 where %2 = %1.%3)",
        "exists (select 1 from %1 where %2 = %1.%3)"
      }; 

      QString conditionTextFormat(qApp->translate(translationContext, conditionTypeTextFormat[conditionType]));
      setConditionText(conditionTextFormat.arg(field->title()));

      QString sql(conditionTypeSql[conditionType]);      
      if (FieldDescription::entitySetField != field->fieldType())
      {
        setSql(sql.arg(field->fieldName()));
      }
      else if (isEmptyCondition == conditionType || isNotEmptyCondition == conditionType)
      {
        setSql(sql.arg(field->entitySetTableName()).arg(field->relationFieldName()).arg(field->entitySetRelationFieldName()));
      }
      else 
      {
        boost::throw_exception(std::invalid_argument("unsupported entity set condition"));          
      }      
    }

    std::size_t UnaryFilterCondition::paramCount() const
    {
      return 0;
    }    

    void UnaryFilterCondition::bindQueryParams(QSqlQuery& /*query*/, std::size_t /*baseParamNo*/, const DatabaseModel& /*databaseModel*/) const
    {
      //nothing here
    }

  } // namespace chrono
} //namespace ma
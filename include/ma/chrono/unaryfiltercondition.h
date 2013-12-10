//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_UNARYFILTERCONDITION_H
#define MA_CHRONO_UNARYFILTERCONDITION_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/onefieldbasedfiltercondition.h>

namespace ma {
namespace chrono {

class UnaryFilterCondition : public OneFieldBasedFilterCondition
{
public:
  enum ConditionType
  {
    isNullCondition,
    isNotNullCondition,
    isEmptyCondition,
    isNotEmptyCondition,
    conditionTypeCount  
  }; // ConditionType

  UnaryFilterCondition(const FieldDescriptionPtr& field, 
      ConditionType conditionType);
  std::size_t paramCount() const;            
  void bindQueryParams(QSqlQuery& query, std::size_t baseParamNo, 
      const DatabaseModel& databaseModel) const;

private:
  ConditionType conditionType_;      
}; // class UnaryFilterCondition    

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_UNARYFILTERCONDITION_H

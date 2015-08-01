//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_BINARYFILTERCONDITION_H
#define MA_CHRONO_BINARYFILTERCONDITION_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <QVariant>
#include <ma/chrono/onefieldbasedfiltercondition.h>

namespace ma {
namespace chrono {

class BinaryFilterCondition : public OneFieldBasedFilterCondition
{
public:
  enum ConditionType
  {
    equalsCondition = 0,
    notEqualCondition,
    greaterCondition,
    lessCondition,
    greaterOrEqualCondition,
    lessOrEqualCondition,
    likeCondition,
    notLikeCondition, 
    containsCondition,
    notContainsCondition,
    conditionTypeCount
  }; // enum ConditionType

  BinaryFilterCondition(const FieldDescriptionPtr& field, 
      ConditionType conditionType, const QString& rightValueText, 
      const QVariant& rightValue);
  std::size_t paramCount() const;      
  void bindQueryParams(QSqlQuery& query, std::size_t baseParamNo, 
      const DatabaseModel& databaseModel) const;

private:
  ConditionType conditionType_;
  QString       rightValueText_;
  QVariant      rightValue_;
}; // class BinaryFilterCondition

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_BINARYFILTERCONDITION_H


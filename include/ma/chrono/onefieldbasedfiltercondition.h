//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_ONEFIELDBASEDFILTERCONDITION_H
#define MA_CHRONO_ONEFIELDBASEDFILTERCONDITION_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/basicfiltercondition.h>
#include <ma/chrono/fielddescription.h>

namespace ma {
namespace chrono {    

class OneFieldBasedFilterCondition : public BasicFilterCondition
{
public:      
  explicit OneFieldBasedFilterCondition(const FieldDescriptionPtr& field)
    : field_(field)
  {
  }

  std::size_t fieldCount() const
  {
    return 1;
  }

protected:
  FieldDescriptionPtr field() const
  {
    field_;
  }

private:
  FieldDescriptionPtr field_;
}; // class OneFieldBasedFilterCondition
    
} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_ONEFIELDBASEDFILTERCONDITION_H

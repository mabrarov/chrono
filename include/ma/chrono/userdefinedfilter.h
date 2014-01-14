//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_USERDEFINEDFILTER_H
#define MA_CHRONO_USERDEFINEDFILTER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/filterexpression.h>

namespace ma {
namespace chrono {

struct UserDefinedFilter
{
  bool enabled;
  FilterExpression filterExpression;

  UserDefinedFilter(bool theEnabled, 
      const FilterExpression& theFilterExpression)
    : enabled(theEnabled)
    , filterExpression(theFilterExpression)
  {
  }

  UserDefinedFilter()
    : enabled(false)
    , filterExpression()
  {
  }

  FilterExpression getUserDefinedFilterExpression() const
  {
    return enabled ? filterExpression : FilterExpression();
  }
}; // class UserDefinedFilter

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_USERDEFINEDFILTER_H

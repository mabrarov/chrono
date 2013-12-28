//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_USERDEFINEDORDER_H
#define MA_CHRONO_USERDEFINEDORDER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/orderexpression.h>

namespace ma {
namespace chrono {

struct UserDefinedOrder
{
  bool enabled;
  OrderExpression orderExpression;

  UserDefinedOrder(bool theEnabled, const OrderExpression& theOrderExpression)
    : enabled(theEnabled)
    , orderExpression(theOrderExpression)
  {
  }

  UserDefinedOrder()
    : enabled(false)
    , orderExpression()
  {
  }

  OrderExpression getUserDefinedOrderExpression() const
  {
    return enabled ? orderExpression : OrderExpression();
  }
}; // class UserDefinedOrder

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_USERDEFINEDORDER_H

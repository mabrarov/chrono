//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_MODEL_RATEAMOUNT_H
#define MA_CHRONO_MODEL_RATEAMOUNT_H

#include <boost/config.hpp>
#include <ma/chrono/model/rateamount_fwd.h>
#include <ma/chrono/model/decimalamount.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

namespace ma {
namespace chrono {
namespace model {

class RateAmount : public DecimalAmount
{
public:
  BOOST_STATIC_CONSTANT(int, fracDiv    = 10000);
  BOOST_STATIC_CONSTANT(int, fracDigits = 4);

  RateAmount()
    : DecimalAmount(fracDiv, fracDigits)          
  {
  }

  explicit RateAmount(qint64 amount)
    : DecimalAmount(fracDiv, fracDigits, amount)          
  {
  }
}; // class RateAmount      

} // namespace model    
} // namespace chrono
} // namespace ma

Q_DECLARE_METATYPE(ma::chrono::model::RateAmount)

#endif // MA_CHRONO_MODEL_RATEAMOUNT_H

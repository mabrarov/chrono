//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_MODEL_CURRENCYAMOUNT_H
#define MA_CHRONO_MODEL_CURRENCYAMOUNT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/config.hpp>
#include <ma/chrono/model/currencyamount_fwd.h>
#include <ma/chrono/model/decimalamount.h>

namespace ma {
namespace chrono {
namespace model {

class CurrencyAmount : public DecimalAmount
{
public:
  BOOST_STATIC_CONSTANT(int, fracDiv    = 100);
  BOOST_STATIC_CONSTANT(int, fracDigits = 2);

  explicit CurrencyAmount()
    : DecimalAmount(fracDiv, fracDigits)          
  {
  }

  explicit CurrencyAmount(qint64 amount)
    : DecimalAmount(fracDiv, fracDigits, amount)          
  {
  }        
}; // class CurrencyAmount      

} // namespace model    
} // namespace chrono
} //namespace ma

Q_DECLARE_METATYPE(ma::chrono::model::CurrencyAmount)

#endif // MA_CHRONO_MODEL_CURRENCYAMOUNT_H

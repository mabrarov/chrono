//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_MODEL_CCD_H
#define MA_CHRONO_MODEL_CCD_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/config.hpp>
#include <QtGlobal>
#include <QString>
#include <QDateTime>
#include <ma/chrono/types.h>
#include <ma/chrono/model/ccd_fwd.h>

namespace ma {
namespace chrono {
namespace model {
 
struct Ccd
{
  BOOST_STATIC_CONSTANT(std::size_t, zeroTaxPeriodMin     = 0);
  BOOST_STATIC_CONSTANT(std::size_t, zeroTaxPeriodMax     = 1000);
  BOOST_STATIC_CONSTANT(std::size_t, paymentTermPeriodMin = 0);
  BOOST_STATIC_CONSTANT(std::size_t, paymentTermPeriodMax = 1000);
  BOOST_STATIC_CONSTANT(std::size_t, numberMaxLen         = 32);
  BOOST_STATIC_CONSTANT(std::size_t, productCodeMaxLen    = 128);
  BOOST_STATIC_CONSTANT(std::size_t, seriesMaxLen         = 128);

  qint64          ccdId;
  QString         number;        
  qint64          contractId;        
  qint64          deppointId;        
  QDate           exportPermDate;        
  OptionalQDate   exportDate;
  OptionalQString productCode;
  OptionalQInt64  duty;
  OptionalQInt64  rate;
  OptionalQString series;
  qint64          ttermId;
  qint64          amount;        
  qint64          createUserId;
  qint64          updateUserId;        
  QDateTime       createTime;
  QDateTime       updateTime;
  qint32          zeroTaxPeriod;
  qint32          paymentTermType;
  bool            paymentControlFlag;
  bool            zeroTaxControlFlag;
  OptionalQDate   paymentTermDate;
  OptionalQInt32  paymentTermPeriod;       
}; // struct Ccd

} // namespace model    
} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_MODEL_CCD_H

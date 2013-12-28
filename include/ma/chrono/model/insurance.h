//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_MODEL_INSURANCE_H
#define MA_CHRONO_MODEL_INSURANCE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/config.hpp>
#include <QtGlobal>
#include <QString>
#include <QDateTime>
#include <ma/chrono/model/insurance_fwd.h>

namespace ma {
namespace chrono {
namespace model {

struct Insurance
{
  BOOST_STATIC_CONSTANT(std::size_t, policyNumberMaxLen = 128);

  qint64    insuranceId;
  QString   policyNumber;
  qint64    contractId;
  qint64    currencyId;
  qint64    amount;
  QDate     signingDate;   
  QDate     startDate;   
  QDate     endDate;   
  qint64    createUserId;
  qint64    updateUserId;
  QDateTime createTime;
  QDateTime updateTime;
}; // struct Country

} // namespace model    
} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_MODEL_INSURANCE_H

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_MODEL_CONTRACT_H
#define MA_CHRONO_MODEL_CONTRACT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/config.hpp>
#include <qglobal.h>
#include <QString>
#include <QDateTime>
#include <ma/chrono/types.h>
#include <ma/chrono/model/bank_fwd.h>

namespace ma {
namespace chrono {
namespace model {

struct Contract
{
  BOOST_STATIC_CONSTANT(std::size_t, contractNumberMaxLen = 128);
  BOOST_STATIC_CONSTANT(std::size_t, passportNumberMaxLen = 128);
  BOOST_STATIC_CONSTANT(std::size_t, remarkMaxLen         = 1024);
  BOOST_STATIC_CONSTANT(std::size_t, closeRemarkMaxLen    = 1024);

  qint64            contractId;
  qint64            residentId;
  qint64            currencyId;
  qint64            counterpartId;
  qint64            bankId;
  QString           number;
  qint64            amount;
  OptionalQString   passportNumber;
  QDate             signingDate;
  QDate             endDate;
  bool              closeMark;
  OptionalQString   closeRemark;
  OptionalQString   remark;
  OptionalQInt64    closeUserId;
  qint64            createUserId;
  qint64            updateUserId;
  OptionalQDateTime closeTime;
  QDateTime         createTime;
  QDateTime         updateTime;
  QDate             passportEndDate;
  QDate             passportSigningDate;
  OptionalQDate     closeDate;
}; // struct Contract

} // namespace model
} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_MODEL_CONTRACT_H

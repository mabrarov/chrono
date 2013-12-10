//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_MODEL_PAYSHEET_H
#define MA_CHRONO_MODEL_PAYSHEET_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/config.hpp>
#include <QtGlobal>
#include <QString>
#include <QDateTime>
#include <ma/chrono/types.h>
#include <ma/chrono/model/paysheet_fwd.h>

namespace ma {
namespace chrono {
namespace model {

struct Paysheet
{
  BOOST_STATIC_CONSTANT(std::size_t, accountNumberMaxLen = 128);
  BOOST_STATIC_CONSTANT(std::size_t, docNumberMaxLen     = 64);
  BOOST_STATIC_CONSTANT(std::size_t, remarkMaxLen        = 1024);

  qint64          paysheetId;        
  QString         docNumber;        
  qint64          createUserId;
  QDateTime       createTime;
  qint64          updateUserId;
  QDateTime       updateTime;
  qint64          contractId;
  OptionalQInt64  ccdId;
  qint64          currencyId;
  qint64          amount;
  QDate           payDate;
  OptionalQString accountNumber;        
  OptionalQString remark;
}; // struct Paysheet

} // namespace model    
} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_MODEL_PAYSHEET_H

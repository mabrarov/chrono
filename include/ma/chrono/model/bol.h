//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_MODEL_BOL_H
#define MA_CHRONO_MODEL_BOL_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/config.hpp>
#include <qglobal.h>
#include <QString>
#include <QDateTime>
#include <ma/chrono/types.h>
#include <ma/chrono/model/bol_fwd.h>

namespace ma {
namespace chrono {
namespace model {

struct Bol
{
  BOOST_STATIC_CONSTANT(std::size_t, bolNumberMaxLen     = 64);
  BOOST_STATIC_CONSTANT(std::size_t, vehicleNumberMaxLen = 32);
  BOOST_STATIC_CONSTANT(std::size_t, placeCountMax       = 100000);
  BOOST_STATIC_CONSTANT(std::size_t, remarkMaxLen        = 1024);

  qint64          bolId;
  qint64          ccdId;
  QString         bolNumber;
  OptionalQString vehicleNumber;        
  qint64          autoenterpriseId;                
  QDate           bolDate;        
  QDate           loadPermDate;
  qint64          placeCount;
  qint64          cargoWeight;
  double          cargoVolume;        
  OptionalQString remark;        
  qint64          createUserId;
  qint64          updateUserId;        
  QDateTime       createTime;
  QDateTime       updateTime;
}; // struct Bol

} // namespace model
} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_MODEL_BOL_H

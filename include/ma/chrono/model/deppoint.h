//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_MODEL_DEPPOINT_H
#define MA_CHRONO_MODEL_DEPPOINT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/config.hpp>
#include <qglobal.h>
#include <QString>
#include <QDateTime>
#include <ma/chrono/types.h>
#include <ma/chrono/model/deppoint_fwd.h>

namespace ma {
namespace chrono {
namespace model {

struct Deppoint
{
  BOOST_STATIC_CONSTANT(std::size_t, fullNameMaxLen  = 1024);
  BOOST_STATIC_CONSTANT(std::size_t, shortNameMaxLen = 64);
  BOOST_STATIC_CONSTANT(std::size_t, remarkMaxLen    = 1024);

  qint64          deppointId;
  QString         fullName;
  QString         shortName;        
  OptionalQString remark;
  qint64          createUserId;
  qint64          updateUserId;
  QDateTime       createTime;
  QDateTime       updateTime;
}; // struct Deppoint

} // namespace model
} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_MODEL_DEPPOINT_H

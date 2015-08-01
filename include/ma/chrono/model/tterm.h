//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_MODEL_TTERM_H
#define MA_CHRONO_MODEL_TTERM_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/config.hpp>
#include <qglobal.h>
#include <QString>
#include <QDateTime>
#include <ma/chrono/types.h>
#include <ma/chrono/model/tterm_fwd.h>

namespace ma {
namespace chrono {
namespace model {

struct Tterm
{
  BOOST_STATIC_CONSTANT(std::size_t, nameMaxLen   = 32);
  BOOST_STATIC_CONSTANT(std::size_t, remarkMaxLen = 128);

  qint64          ttermId;
  QString         name;
  OptionalQString remark;        
  qint64          createUserId;
  qint64          updateUserId;
  QDateTime       createTime;
  QDateTime       updateTime;
}; // struct Tterm

} // namespace model    
} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_MODEL_TTERM_H

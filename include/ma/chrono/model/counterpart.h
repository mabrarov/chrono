//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_MODEL_COUNTERPART_H
#define MA_CHRONO_MODEL_COUNTERPART_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/config.hpp>
#include <QtGlobal>
#include <QString>
#include <QDateTime>
#include <ma/chrono/types.h>
#include <ma/chrono/model/counterpart_fwd.h>

namespace ma {
namespace chrono {
namespace model {

struct Counterpart
{
  BOOST_STATIC_CONSTANT(std::size_t, fullNameMaxLen       = 1024);
  BOOST_STATIC_CONSTANT(std::size_t, fullLatinNameMaxLen  = 1024);
  BOOST_STATIC_CONSTANT(std::size_t, shortNameMaxLen      = 16);
  BOOST_STATIC_CONSTANT(std::size_t, shortLatinNameMaxLen = 16);

  qint64    counterpartId;
  qint64    countryId;
  QString   fullName;
  QString   fullLatinName;
  QString   shortName; 
  QString   shortLatinName;
  qint64    createUserId;
  qint64    updateUserId;
  QDateTime createTime;
  QDateTime updateTime;
}; // struct Counterpart

} // namespace model
} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_MODEL_COUNTERPART_H

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_MODEL_COUNTRY_H
#define MA_CHRONO_MODEL_COUNTRY_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/config.hpp>
#include <QtGlobal>
#include <QString>
#include <QDateTime>
#include <ma/chrono/types.h>
#include <ma/chrono/model/country_fwd.h>

namespace ma {
namespace chrono {
namespace model {

struct Country
{
  BOOST_STATIC_CONSTANT(std::size_t, fullNameMaxLen            = 128);
  BOOST_STATIC_CONSTANT(std::size_t, shortNameMaxLen           = 10);
  BOOST_STATIC_CONSTANT(std::size_t, trigramCyrillicCodeMaxLen = 3);
  BOOST_STATIC_CONSTANT(std::size_t, trigramLatinCodeMaxLen    = 3);
  BOOST_STATIC_CONSTANT(std::size_t, twoLetterLatinCodeMaxLen  = 2);

  qint64    countryId;
  QString   fullName;
  QString   shortName;
  QString   trigramCyrillicCode;
  QString   trigramLatinCode;
  QString   twoLetterLatinCode;
  qint32    numericCode;
  qint64    createUserId;
  qint64    updateUserId;
  QDateTime createTime;
  QDateTime updateTime;
}; // struct Country

} // namespace model
} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_MODEL_COUNTRY_H

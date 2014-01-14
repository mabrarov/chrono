//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_MODEL_RESIDENT_H
#define MA_CHRONO_MODEL_RESIDENT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/config.hpp>
#include <qglobal.h>
#include <QString>
#include <QDateTime>
#include <ma/chrono/types.h>
#include <ma/chrono/model/resident_fwd.h>

namespace ma {
namespace chrono {
namespace model {

struct Resident
{
  BOOST_STATIC_CONSTANT(std::size_t, fullNameMaxLen        = 1024);        
  BOOST_STATIC_CONSTANT(std::size_t, shortNameMaxLen       = 16);
  BOOST_STATIC_CONSTANT(std::size_t, regionNameMaxLen      = 64);
  BOOST_STATIC_CONSTANT(std::size_t, cityNameMaxLen        = 64);
  BOOST_STATIC_CONSTANT(std::size_t, streetNameMaxLen      = 128);
  BOOST_STATIC_CONSTANT(std::size_t, advHouseNumberMaxLen  = 8);
  BOOST_STATIC_CONSTANT(std::size_t, caseNumberMaxLen      = 8);
  BOOST_STATIC_CONSTANT(std::size_t, advOfficeNumberMaxLen = 8);
  BOOST_STATIC_CONSTANT(std::size_t, zipCodeMaxLen         = 16);
  BOOST_STATIC_CONSTANT(std::size_t, registryNumberMaxLen  = 64);
  BOOST_STATIC_CONSTANT(std::size_t, intbMaxLen            = 64);
  BOOST_STATIC_CONSTANT(std::size_t, crsaMaxLen            = 64);

  qint64          residentId;
  qint64          countryId;        
  QString         fullName;
  QString         shortName; 
  OptionalQString regionName;
  OptionalQString cityName;
  OptionalQString streetName;
  OptionalQInt32  houseNumber;
  OptionalQString advHouseNumber;
  OptionalQString caseNumber;
  OptionalQInt32  officeNumber;
  OptionalQString advOfficeNumber;
  OptionalQString zipCode;
  OptionalQString registryNumber;
  OptionalQString intb;
  OptionalQString crsa;
  OptionalQDate   sreDate;
  qint64          createUserId;
  qint64          updateUserId;
  QDateTime       createTime;
  QDateTime       updateTime;
}; // struct Resident

} // namespace model    
} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_MODEL_RESIDENT_H

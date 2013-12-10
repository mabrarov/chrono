//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_MODEL_AUTOENTERPRISE_H
#define MA_CHRONO_MODEL_AUTOENTERPRISE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/config.hpp>
#include <QtGlobal>
#include <QString>
#include <QDateTime>
#include <ma/chrono/types.h>
#include <ma/chrono/model/autoenterprise_fwd.h>

namespace ma {
namespace chrono {
namespace model {

struct Autoenterprise
{
  BOOST_STATIC_CONSTANT(std::size_t, fullNameMaxLen  = 128);
  BOOST_STATIC_CONSTANT(std::size_t, shortNameMaxLen = 64);
  BOOST_STATIC_CONSTANT(std::size_t, remarkMaxLen    = 128);

  qint64          autoenterpriseId;
  QString         fullName;
  QString         shortName;        
  OptionalQString remark;
  qint64          createUserId;
  qint64          updateUserId;
  QDateTime       createTime;
  QDateTime       updateTime;
}; // struct Autoenterprise

} // namespace model
} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_MODEL_AUTOENTERPRISE_H

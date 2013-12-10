//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_MODEL_USER_H
#define MA_CHRONO_MODEL_USER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/config.hpp>
#include <QtGlobal>
#include <QString>
#include <QDateTime>
#include <ma/chrono/types.h>
#include <ma/chrono/model/user_fwd.h>

namespace ma {
namespace chrono {
namespace model {

struct User
{
  BOOST_STATIC_CONSTANT(std::size_t, userNameMaxLen   = 64);
  BOOST_STATIC_CONSTANT(std::size_t, firstNameMaxLen  = 64);
  BOOST_STATIC_CONSTANT(std::size_t, lastNameMaxLen   = 128);
  BOOST_STATIC_CONSTANT(std::size_t, middleNameMaxLen = 128);
  BOOST_STATIC_CONSTANT(std::size_t, passwordMaxLen   = 32);

  qint64          userId;
  QString         userName;
  QString         firstName;
  QString         lastName;
  OptionalQString middleName;
  OptionalQInt64  createUserId;
  OptionalQInt64  updateUserId;
  QDateTime       createTime;
  QDateTime       updateTime;
}; // struct User

} // namespace model    
} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_MODEL_USER_H

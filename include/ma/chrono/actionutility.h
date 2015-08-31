//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_ACTIONUTILITY_H
#define MA_CHRONO_ACTIONUTILITY_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/noncopyable.hpp>
#include <ma/chrono/types.h>

namespace ma {
namespace chrono {

class ActionUtility : private boost::noncopyable
{
public:
  static void enable(const QActionList& actions, bool enabled = true);
  static void show(const QActionList& actions, bool visible = true);

private:
  ActionUtility(); 
}; // class ModelUtility

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_ACTIONUTILITY_H

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_SQLUTILITY_H
#define MA_CHRONO_SQLUTILITY_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/noncopyable.hpp>
#include <QSqlQuery>
#include <ma/chrono/types.h>

namespace ma {
namespace chrono {

class SqlUtility : private boost::noncopyable
{
public:
  static OptionalQString getOptionalQString(const QSqlQuery& query, int index)
  {
    if (query.isNull(index))
    {
      return OptionalQString();
    }
    return query.value(index).toString();
  }

private:
  SqlUtility(); 
}; // class WidgetUtility

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_SQLUTILITY_H

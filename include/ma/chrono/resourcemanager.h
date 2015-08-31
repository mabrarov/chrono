//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_RESOURCEMANAGER_H
#define MA_CHRONO_RESOURCEMANAGER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/utility.hpp>
#include <QString>
#include <QMap>
#include <QIcon>
#include <ma/chrono/resourcemanager_fwd.h>

namespace ma {
namespace chrono {

class ResourceManager : private boost::noncopyable
{
public:
  ResourceManager();
  ~ResourceManager();
  QIcon getIcon(const QString& fileName);

private:      
  QMap<QString, QIcon> cachedIcons_;
}; // class ResourceManager

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_RESOURCEMANAGER_H

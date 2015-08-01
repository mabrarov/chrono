//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ma/chrono/resourcemanager.h>

namespace ma {
namespace chrono {

ResourceManager::ResourceManager()
  : cachedIcons_()
{
}

ResourceManager::~ResourceManager()
{
}

QIcon ResourceManager::getIcon(const QString& fileName)
{
  QIcon icon(cachedIcons_.value(fileName));
  if (icon.isNull())
  {
    icon = QIcon(fileName);
    cachedIcons_.insert(fileName, icon);
  }
  return icon;      
}
    
} // namespace chrono
} // namespace ma

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_RESOURCEMANAGER_FWD_H
#define MA_CHRONO_RESOURCEMANAGER_FWD_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/shared_ptr.hpp>

namespace ma {
namespace chrono {

class ResourceManager;
typedef boost::shared_ptr<ResourceManager> ResourceManagerPtr;

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_RESOURCEMANAGER_FWD_H

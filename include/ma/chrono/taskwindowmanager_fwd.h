//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_TASKWINDOWMANAGER_FWD_H
#define MA_CHRONO_TASKWINDOWMANAGER_FWD_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace ma {
namespace chrono {

class TaskWindowManager;
typedef boost::shared_ptr<TaskWindowManager> TaskWindowManagerPtr;
typedef boost::weak_ptr<TaskWindowManager>   TaskWindowManagerWeakPtr;

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_TASKWINDOWMANAGER_FWD_H

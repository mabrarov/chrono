//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_TASKWINDOW_H
#define MA_CHRONO_TASKWINDOW_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/commanddialog.h>
#include <ma/chrono/taskwindow_fwd.h>

namespace ma {
namespace chrono {

class TaskWindow : public CommandDialog
{
  Q_OBJECT

public:
  TaskWindow();
  ~TaskWindow();      

private:
  Q_DISABLE_COPY(TaskWindow)      
}; // class TaskWindow

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_TASKWINDOW_H

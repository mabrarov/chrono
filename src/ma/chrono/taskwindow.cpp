/*
TRANSLATOR ma::chrono::TaskWindow
*/

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ma/chrono/taskwindow.h>

namespace ma {
namespace chrono {

TaskWindow::TaskWindow()
  : CommandDialog()
{ 
  setWindowFlags(Qt::Window);
  setModal(false);
  resize(600, 400);
}

TaskWindow::~TaskWindow()
{
}

} // namespace chrono
} // namespace ma

/*
TRANSLATOR ma::chrono::TaskWindow
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <ma/chrono/taskwindow.h>

namespace ma
{
  namespace chrono
  {
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
} //namespace ma

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_COMMANDDIALOG_H
#define MA_CHRONO_COMMANDDIALOG_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <QDialog>
#include <ma/chrono/commandbar_fwd.h>
#include <ma/chrono/taskwindow_fwd.h>

namespace ma {
namespace chrono {

class CommandDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CommandDialog(QWidget* parent = 0);
  ~CommandDialog();
  CommandBar* commandBar();
  QWidget*    contents();

private:
  Q_DISABLE_COPY(CommandDialog)

  CommandBar* commandBar_;
  QWidget*    contents_;
}; // class CommandDialog

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_COMMANDDIALOG_H

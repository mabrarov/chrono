/*
TRANSLATOR ma::chrono::CommandDialog
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <QVBoxLayout>
#include <ma/chrono/commandbar.h>
#include <ma/chrono/commanddialog.h>

namespace ma
{
  namespace chrono
  {
    CommandDialog::CommandDialog(QWidget* parent)
      : QDialog(parent)
    { 
      //setSizeGripEnabled(true);
      QVBoxLayout* layout = new QVBoxLayout(this);
      layout->setContentsMargins(0, 0, 0, 0);
      layout->setSpacing(0);
      commandBar_ = new CommandBar(this);      
      layout->addWidget(commandBar_);

      contents_ = new QWidget(this);
      layout->addWidget(contents_);
    }

    CommandDialog::~CommandDialog()
    {
    }

    CommandBar* CommandDialog::commandBar()
    {
      return commandBar_;
    }

    QWidget* CommandDialog::contents()
    {
      return contents_;
    }

  } // namespace chrono
} // namespace ma

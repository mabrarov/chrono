/*
TRANSLATOR ma::chrono::CommandDialog
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
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
} //namespace ma

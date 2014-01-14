/*
TRANSLATOR ma::chrono::ListSelectDialog
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <QAbstractItemModel>
#include <QIcon>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTableView>
#include <QItemSelectionModel>
#include <ma/chrono/commandbar.h>
#include <ma/chrono/listpage.h>
#include <ma/chrono/listselectdialog.h>

namespace ma {
namespace chrono {

ListSelectDialog::ListSelectDialog(
    QWidget* parent, std::auto_ptr<ListPage> listPage)
  : CommandDialog(parent)
{
  setWindowIcon(listPage->windowIcon());
  setWindowTitle(tr("Select - %1").arg(listPage->windowTitle()));

  setWindowFlags(static_cast<Qt::WindowFlags>(
      Qt::Dialog | Qt::WindowCloseButtonHint));
  setModal(true);
  resize(600, 400);

  QWidget* contents = this->contents();
  QVBoxLayout* contentsLayout = new QVBoxLayout(contents);
  contentsLayout->setContentsMargins(0, 0, 0, 0);
  contentsLayout->setSpacing(0);

  CommandBar* commandBar = this->commandBar();
  commandBar->addCommands(listPage->primaryActions());
  commandBar->addCommand(listPage->helpAction().get(), CommandBar::tray, false);
  if (QAction* customizeAction = listPage->customizeAction())
  {
    commandBar->addCommand(customizeAction, CommandBar::tray, false);
  }
      
  listPage->setParent(contents);      
  contentsLayout->addWidget(listPage.get());
  listPage_ = listPage.release();
            
  QWidget* footer = new QWidget(contents);
  contentsLayout->addWidget(footer);
  QVBoxLayout* footerLayout = new QVBoxLayout(footer);
  QDialogButtonBox* buttonBox = new QDialogButtonBox(footer);
  footerLayout->addWidget(buttonBox);
  buttonBox->setStandardButtons(
      QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  QObject::connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
  QObject::connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));

  okButton_ = buttonBox->button(QDialogButtonBox::Ok);
  okButton_->setEnabled(false);
      
  QObject::connect(listPage_, 
      SIGNAL(doubleClicked(const QModelIndex&)), 
      SLOT(on_listPage_doubleClicked(const QModelIndex&)));
  QObject::connect(listPage_, 
      SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), 
      SLOT(on_listPage_currentChanged(const QModelIndex&, const QModelIndex&)));
  QObject::connect(listPage_, SIGNAL(needToClose()), SLOT(reject()));
}

ListSelectDialog::~ListSelectDialog()
{
}

ListPage* ListSelectDialog::listPage() const
{
  return listPage_;
}

void ListSelectDialog::on_listPage_currentChanged(
    const QModelIndex& current, const QModelIndex& /*previous*/)
{
  if (current.isValid())
  {
    okButton_->setEnabled(0 <= current.row());
  } 
  else
  {
    okButton_->setEnabled(false);
  }      
}

void ListSelectDialog::on_listPage_windowTitleUpdated()
{
    setWindowTitle(listPage_->windowTitle());
}

void ListSelectDialog::on_listPage_doubleClicked(const QModelIndex& /*index*/)
{
  okButton_->click();
}

} // namespace chrono
} // namespace ma

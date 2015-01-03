/*
TRANSLATOR ma::chrono::DatabaseConnectDialog
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <QPushButton>
#include <ma/chrono/types.h>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/databaseconnectdialog.h>

namespace ma {
namespace chrono {

DatabaseConnectDialog::DatabaseConnectDialog(
    const ResourceManagerPtr& resourceManager, QWidget* parent)
  : QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint)
{      
  ui_.setupUi(this);
  setWindowIcon(resourceManager->getIcon(chronoIconName));      
  QObject::connect(ui_.loginEdit, SIGNAL(textChanged(const QString&)), SLOT(on_loginEdit_textChanged(const QString&)));
  QObject::connect(ui_.databaseNameList, SIGNAL(currentRowChanged(int)), SLOT(on_databaseNameList_currentRowChanged(int)));
      
  QPushButton* okButton = ui_.buttonBox->button(QDialogButtonBox::Ok);
  QObject::connect(ui_.databaseNameEdit, SIGNAL(returnPressed()), okButton, SLOT(click()));
  QObject::connect(ui_.loginEdit, SIGNAL(returnPressed()), okButton, SLOT(click()));
  QObject::connect(ui_.passwordEdit, SIGNAL(returnPressed()), okButton, SLOT(click()));
      
  updateState();
}

DatabaseConnectDialog::~DatabaseConnectDialog()
{
} 

QString DatabaseConnectDialog::login() const
{
  return ui_.loginEdit->text();
}

QString DatabaseConnectDialog::password() const
{
  return ui_.passwordEdit->text();
}

void DatabaseConnectDialog::prepare()
{
  ui_.databaseNameList->setFocus(Qt::MouseFocusReason);
  if (0 > ui_.databaseNameList->currentRow())
  {
    if (ui_.databaseNameList->count())
    {
      ui_.databaseNameList->setCurrentRow(0);
      ui_.loginEdit->setFocus(Qt::MouseFocusReason);
      ui_.loginEdit->selectAll();
    }
  }
  else
  {
    ui_.loginEdit->setFocus(Qt::MouseFocusReason);
    ui_.loginEdit->selectAll();
  }
  ui_.passwordEdit->setText(QString());
}

int DatabaseConnectDialog::currentRow() const
{
  return ui_.databaseNameList->currentRow();
}

OptionalQString DatabaseConnectDialog::currentConnection() const
{
  int row = ui_.databaseNameList->currentRow();
  if (row >= 0)
  {
    return ui_.databaseNameList->item(row)->text();
  }
  return OptionalQString();
}

void DatabaseConnectDialog::on_loginEdit_textChanged(const QString& /*text*/)
{
  updateState();
}

void DatabaseConnectDialog::on_databaseNameList_currentRowChanged(int /*currentRow*/)
{
  updateState();
}

void DatabaseConnectDialog::updateState()
{
  bool enabled = (!ui_.loginEdit->text().isEmpty()) &&
    (-1 < ui_.databaseNameList->currentRow());
  ui_.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enabled);
}

} // namespace chrono
} // namespace ma

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_DATABASECONNECTDIALOG_H
#define MA_CHRONO_DATABASECONNECTDIALOG_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <QWidget>
#include <ma/chrono/types.h>
#include <ma/chrono/resourcemanager_fwd.h>
#include <ui_databaseconnectdialog.h>
#include <ma/chrono/databaseconnectdialog_fwd.h>

namespace ma {
namespace chrono {

class DatabaseConnectDialog : public QDialog
{
  Q_OBJECT

public:
  DatabaseConnectDialog(
      const ResourceManagerPtr& resourceManager, 
      QWidget* parent);
  ~DatabaseConnectDialog();

  QString login() const;
  QString password() const;
  void prepare();
  int currentRow() const;
  OptionalQString currentConnection() const;

private:
  Q_DISABLE_COPY(DatabaseConnectDialog)
  void updateState();

  Ui::databaseConnectDialog ui_;

private slots:      
  void on_loginEdit_textChanged(const QString& text);
  void on_databaseNameList_currentRowChanged(int currentRow);      
}; // class DatabaseConnectDialog

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_DATABASECONNECTDIALOG_H

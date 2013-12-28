//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_LISTSELECTDIALOG_H
#define MA_CHRONO_LISTSELECTDIALOG_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <memory>
#include <ma/chrono/listpage_fwd.h>
#include <ma/chrono/commanddialog.h>

QT_BEGIN_NAMESPACE
class QPushButton;
class QModelIndex;
QT_END_NAMESPACE

namespace ma {
namespace chrono {

class ListSelectDialog : public CommandDialog
{
  Q_OBJECT

public:
  ListSelectDialog(QWidget* parent, std::auto_ptr<ListPage> listPage);
  ~ListSelectDialog();
  ListPage* listPage() const;

private:
  Q_DISABLE_COPY(ListSelectDialog)
      
  ListPage* listPage_;
  QPushButton* okButton_;
          
private slots:
  void on_listPage_currentChanged(const QModelIndex& current, 
      const QModelIndex& previous);
  void on_listPage_windowTitleUpdated();
  void on_listPage_doubleClicked(const QModelIndex& index);
}; // class ListSelectDialog

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_LISTSELECTDIALOG_H

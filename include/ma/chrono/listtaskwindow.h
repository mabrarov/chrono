//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_LISTTASKWINDOW_H
#define MA_CHRONO_LISTTASKWINDOW_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <memory>
#include <ma/chrono/listpage_fwd.h>
#include <ma/chrono/taskwindow.h>

QT_BEGIN_NAMESPACE
class QModelIndex;  
QT_END_NAMESPACE

namespace ma {
namespace chrono {

class ListTaskWindow : public TaskWindow
{
  Q_OBJECT

public:
  explicit ListTaskWindow(std::auto_ptr<ListPage> listPage);
  ~ListTaskWindow();
  ListPage* listPage() const;

private:
  Q_DISABLE_COPY(ListTaskWindow)  

  ListPage* listPage_;

private slots:
  void on_listPage_windowTitleUpdated();
  void on_listPage_doubleClicked(const QModelIndex& index);
}; // class ListTaskWindow

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_LISTTASKWINDOW_H

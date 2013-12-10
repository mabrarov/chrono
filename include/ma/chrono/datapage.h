//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_DATAPAGE_H
#define MA_CHRONO_DATAPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/page.h>
#include <ma/chrono/databasemodel_fwd.h>

namespace ma {
namespace chrono {

class DataPage : public Page
{
  Q_OBJECT    

public:      
  DataPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, QWidget* parent = 0);
  ~DataPage();

  DatabaseModelPtr databaseModel() const;    
  virtual void refresh();
  void updateWindowTitle(const QString& title);

signals:      
  void needToClose();
  void windowTitleUpdated();

private:
  Q_DISABLE_COPY(DataPage)

  DatabaseModelPtr databaseModel_;
}; // class DataPage 

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_DATAPAGE_H

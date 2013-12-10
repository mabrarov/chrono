/*
TRANSLATOR ma::chrono::DataPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <ma/chrono/datapage.h>

namespace ma
{
  namespace chrono
  {    
    DataPage::DataPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, QWidget* parent)
      : Page(resourceManager, taskWindowManager, helpAction, parent)
      , databaseModel_(databaseModel)
    {
    }

    DataPage::~DataPage()
    {
    }    

    DatabaseModelPtr DataPage::databaseModel() const
    {
      return databaseModel_;
    }

    void DataPage::refresh()
    {
      //do nothing here
    }

    void DataPage::updateWindowTitle(const QString& title)
    {
      setWindowTitle(title);
      emit windowTitleUpdated();
    }

  } // namespace chrono
} //namespace ma
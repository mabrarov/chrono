/*
TRANSLATOR ma::chrono::DataPage
*/

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ma/chrono/datapage.h>

namespace ma {
namespace chrono {

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
} // namespace ma

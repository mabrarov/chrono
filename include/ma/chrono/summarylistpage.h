//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_SUMMARYLISTPAGE_H
#define MA_CHRONO_SUMMARYLISTPAGE_H

#include <ma/chrono/listpage.h>

namespace ma {
namespace chrono {

class SummaryListPage : public ListPage
{
  Q_OBJECT

public:
  SummaryListPage(const boost::shared_ptr<ResourceManager>& resourceManager,
      const boost::weak_ptr<TaskWindowManager>& taskWindowManager,
      const boost::shared_ptr<DatabaseModel>& databaseModel,
      const QActionSharedPtr& helpAction,
      QWidget* parent = 0);
  ~SummaryListPage();
  std::auto_ptr<ListPage> clone(QWidget* parent = 0);

private:
  Q_DISABLE_COPY(SummaryListPage)                      
}; // class SummaryListPage

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_SUMMARYLISTPAGE_H

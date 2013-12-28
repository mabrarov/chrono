//
// Copyright (c) 2009-2010 Social Technologies (abrarov@gmail.com)
//

#ifndef MA_CHRONO_SUMMARYLISTPAGE_H
#define MA_CHRONO_SUMMARYLISTPAGE_H

#include <ma/chrono/listpage.h>

namespace ma
{
  namespace chrono
  {
    class SummaryListPage : public ListPage
    {
      Q_OBJECT

    public:
      explicit SummaryListPage(const boost::shared_ptr<ResourceManager>& resourceManager,
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
} //namespace ma

#endif // MA_CHRONO_SUMMARYLISTPAGE_H

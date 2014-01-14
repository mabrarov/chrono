//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_SUMMARYPAGE_H
#define MA_CHRONO_SUMMARYPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/datapage.h>
#include <ui_summarypage.h>

namespace ma {
namespace chrono {

class SummaryPage : public DataPage
{
  Q_OBJECT

public:      
  SummaryPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, QWidget* parent = 0);
  ~SummaryPage();
  QActionList primaryActions() const;
  QAction* customizeAction() const;
  QActionList contextActions() const;
            
private:
  Q_DISABLE_COPY(SummaryPage)
                      
  QActionList primaryActions_;
  QAction* customizeAction_;
  QActionList contextActions_;
  QActionList dataAwareActions_;
  Ui::summaryPage ui_;

private slots:      
  //void on_openInNewWindowAction_triggered();            
  void on_headerContextMenu_requested(const QPoint& pos);
  void on_databaseModel_connectionStateChanged();
}; // class SummaryPage

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_SUMMARYPAGE_H

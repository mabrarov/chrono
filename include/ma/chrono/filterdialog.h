//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_FILTERDIALOG_H
#define MA_CHRONO_FILTERDIALOG_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/resourcemanager_fwd.h>
#include <ma/chrono/taskwindowmanager_fwd.h>
#include <ma/chrono/databasemodel_fwd.h>
#include <ma/chrono/tabledescription.h>
#include <ma/chrono/filterexpression.h>
#include <ui_filterdialog.h>
#include <ma/chrono/types.h>
#include <ma/chrono/conditiondialog_fwd.h>
#include <ma/chrono/filterdialog_fwd.h>

namespace ma {
namespace chrono {

class FilterDialog : public QDialog
{
  Q_OBJECT

public:     
  FilterDialog(const ResourceManagerPtr& resourceManager, 
      const TableDescription& tableDescription, 
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction,
      QWidget* parent);
  ~FilterDialog();      
  void setFilterExpression(const FilterExpression& filterExpression);
  FilterExpression filterExpression() const;
      
private:
  Q_DISABLE_COPY(FilterDialog)
  void updateState();   
  void addFilterExpressionItem(const FilterExpression::ItemPtr& item);

  Ui::filterDialog ui_;
  TableDescription tableDescription_;
  FilterExpression filterExpression_;
  ConditionDialog* conditionDialog_;

private slots:
  void on_addBtn_clicked(bool checked = false);
  void on_removeBtn_clicked(bool checked = false);
  void on_conditionList_itemSelectionChanged();
}; //class FilterDialog

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_FILTERDIALOG_H

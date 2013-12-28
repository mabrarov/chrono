//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_SORTDIALOG_H
#define MA_CHRONO_SORTDIALOG_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <QVector>
#include <ma/chrono/resourcemanager_fwd.h>
#include <ma/chrono/taskwindowmanager_fwd.h>
#include <ma/chrono/databasemodel_fwd.h>
#include <ma/chrono/tabledescription.h>
#include <ma/chrono/orderexpression.h>
#include <ui_sortdialog.h>
#include <ma/chrono/types.h>
#include <ma/chrono/sortdialog_fwd.h>

namespace ma {
namespace chrono {

class SortDialog : public QDialog
{
  Q_OBJECT

public:            
  SortDialog(const ResourceManagerPtr& resourceManager, 
      const TableDescription& tableDescription, 
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& /*helpAction*/,
      QWidget* parent);
  ~SortDialog();
  void setOrderExpression(const OrderExpression& orderExpression);
  OrderExpression orderExpression() const;

private:
  Q_DISABLE_COPY(SortDialog)

  void updateState();  
  void addAvailableField(const FieldDescriptionPtr& field);
  void insertAvailableField(int row, const FieldDescriptionPtr& field);
  void removeAvailableField(int row);
  void addOrderItem(const OrderExpression::ItemPtr& orderItem);
  void removeOrderItem(int row);     
  void fillAvailableFieldList(
      const QVector<FieldDescriptionPtr>& availableFields);
  void fillOrderExpressionList(const OrderExpression& orderExpression);
  static QVector<FieldDescriptionPtr> getAvalableFields(
      const TableDescription& tableDescription, 
      const OrderExpression& orderExpression);

  Ui::sortDialog ui_;
  TableDescription tableDescription_;
  QVector<FieldDescriptionPtr> availableFields_;
  OrderExpression orderExpression_;

private slots:
  void on_availableFieldList_currentRowChanged(int currentRow);
  void on_orderExpressionList_currentRowChanged(int currentRow);
  void on_addAscBtn_clicked(bool checked);
  void on_addDescBtn_clicked(bool checked);
  void on_moveUpBtn_clicked(bool checked);
  void on_moveDownBtn_clicked(bool checked);
  void on_removeBtn_clicked(bool checked);
}; // class SortDialog

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_SORTDIALOG_H

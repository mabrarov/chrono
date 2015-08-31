/*
TRANSLATOR ma::chrono::SortDialog
*/

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <memory>
#include <boost/make_shared.hpp>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/sortdialog.h>

namespace ma {
namespace chrono {

SortDialog::SortDialog(const ResourceManagerPtr& resourceManager, 
    const TableDescription& tableDescription, 
    const TaskWindowManagerWeakPtr& /*taskWindowManager*/,
    const DatabaseModelPtr& /*databaseModel*/,
    const QActionSharedPtr& /*helpAction*/,
    QWidget* parent)
  : QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint)
  , tableDescription_(tableDescription)      
{      
  ui_.setupUi(this);
  setWindowIcon(resourceManager->getIcon(chronoIconName));      
      
  ui_.addAscBtn->setIcon(resourceManager->getIcon(moveRightIconName));
  ui_.addDescBtn->setIcon(resourceManager->getIcon(moveRightIconName));
  ui_.moveUpBtn->setIcon(resourceManager->getIcon(moveUpIconName));
  ui_.moveDownBtn->setIcon(resourceManager->getIcon(moveDownIconName));
  ui_.removeBtn->setIcon(resourceManager->getIcon(moveLeftIconName));
            
  updateState();
}

SortDialog::~SortDialog()
{
}    

void SortDialog::setOrderExpression(const OrderExpression& orderExpression)
{      
  QVector<FieldDescriptionPtr> availableFields = 
      getAvalableFields(tableDescription_, orderExpression);            
  fillAvailableFieldList(availableFields);
  fillOrderExpressionList(orderExpression);
}

OrderExpression SortDialog::orderExpression() const
{
  return orderExpression_;
}

void SortDialog::updateState()
{      
  {
    bool fieldSelected = ui_.availableFieldList->currentRow() >= 0;
    bool hasFields = ui_.availableFieldList->count() > 0;
    bool canAdd = fieldSelected && hasFields;
    ui_.addAscBtn->setEnabled(canAdd);
    ui_.addDescBtn->setEnabled(canAdd);
  }  
  {
    int  selectedRow = ui_.orderExpressionList->currentRow();    
    bool fieldSelected = selectedRow >= 0;    
    int  rowCount = ui_.orderExpressionList->count();
    bool hasFields = rowCount > 0;
    bool hasFieldsToReorder = rowCount > 1;
    ui_.removeBtn->setEnabled(hasFields && fieldSelected);
    ui_.moveUpBtn->setEnabled(hasFieldsToReorder && (selectedRow > 0));
    ui_.moveDownBtn->setEnabled(hasFieldsToReorder && fieldSelected 
        && (selectedRow < rowCount - 1));
  }
}

void SortDialog::addAvailableField(const FieldDescriptionPtr& field)
{      
  availableFields_ << field;
  ui_.availableFieldList->addItem(field->title());
}

void SortDialog::insertAvailableField(int row, const FieldDescriptionPtr& field)
{      
  availableFields_.insert(row, field);      
  ui_.availableFieldList->insertItem(row, field->title());
}

void SortDialog::removeAvailableField(int row)
{
  availableFields_.remove(row);
  std::auto_ptr<QListWidgetItem>(ui_.availableFieldList->takeItem(row));
}   

void SortDialog::addOrderItem(const OrderExpression::ItemPtr& orderItem)
{      
  orderExpression_.addItem(orderItem);      
  ui_.orderExpressionList->addItem(orderItem->orderText());     
}

void SortDialog::removeOrderItem(int row)
{
  orderExpression_.removeItem(row);
  std::auto_ptr<QListWidgetItem>(ui_.orderExpressionList->takeItem(row));
}

void SortDialog::fillAvailableFieldList(
    const QVector<FieldDescriptionPtr>& availableFields)
{ 
  typedef QVector<FieldDescriptionPtr>::const_iterator iterator;
  ui_.availableFieldList->clear();
  availableFields_.clear();
  for (iterator i = availableFields.begin(), end = availableFields.end(); 
      i != end; ++i)
  {        
    addAvailableField(*i);
  }      
}

void SortDialog::fillOrderExpressionList(const OrderExpression& orderExpression)
{      
  typedef OrderExpression::ItemList::const_iterator iterator;
  OrderExpression::ItemList orderItems = orderExpression.items();       
  ui_.orderExpressionList->clear();
  orderExpression_.clear();
  for (iterator i = orderItems.begin(), end = orderItems.end(); i != end; ++i)
  {
    addOrderItem(*i);
  }                 
}

QVector<FieldDescriptionPtr> SortDialog::getAvalableFields(
    const TableDescription& tableDescription, 
    const OrderExpression& orderExpression)
{      
  typedef TableDescription::FieldList::const_iterator iterator;
  QVector<FieldDescriptionPtr> result;
  TableDescription::FieldList fields = tableDescription.fields();            
  for (iterator i = fields.begin(), end = fields.end(); i != end; ++i)
  {
    FieldDescriptionPtr field = *i;
    if (!orderExpression.containsField(field))
    {
      switch (field->fieldType())
      {
      case FieldDescription::boolField:
      case FieldDescription::currencyAmountField:
      case FieldDescription::dateField:
      case FieldDescription::dateTimeField:        
      case FieldDescription::intField:
      case FieldDescription::rateAmountField:
      case FieldDescription::stringField:
        result << field;
        break;
      }
    }        
  }
  return result;
}

void SortDialog::on_availableFieldList_currentRowChanged(int /*currentRow*/)
{
  updateState();
}

void SortDialog::on_orderExpressionList_currentRowChanged(int /*currentRow*/)
{
  updateState();
}    

void SortDialog::on_addAscBtn_clicked(bool /*checked*/)
{
  int row = ui_.availableFieldList->currentRow();
  if (0 <= row)
  {
    OrderExpression::ItemPtr orderItem = 
        boost::make_shared<OrderExpression::Item>(
            availableFields_.at(row), OrderExpression::Item::ascendingOrder);
    addOrderItem(orderItem);
    removeAvailableField(row);
    updateState();
  }      
}

void SortDialog::on_addDescBtn_clicked(bool /*checked*/)
{
  int row = ui_.availableFieldList->currentRow();
  if (0 <= row)
  {
    OrderExpression::ItemPtr orderItem = 
        boost::make_shared<OrderExpression::Item>(
            availableFields_.at(row), OrderExpression::Item::descendingOrder);
    addOrderItem(orderItem);
    removeAvailableField(row);
    updateState();
  }
}

void SortDialog::on_moveUpBtn_clicked(bool /*checked*/)
{
  int row = ui_.orderExpressionList->currentRow();
  if (1 <= row)
  {
    //todo
    updateState();
  }
}

void SortDialog::on_moveDownBtn_clicked(bool /*checked*/)
{
  int row = ui_.orderExpressionList->currentRow();
  int rowCount = ui_.orderExpressionList->count();
  if ((0 <= row) && (rowCount - 1 > row))
  {
    //todo
    updateState();
  }
}

void SortDialog::on_removeBtn_clicked(bool /*checked*/)
{
  int row = ui_.orderExpressionList->currentRow();
  if (0 <= row)
  {
    FieldDescriptionPtr field = orderExpression_.items().at(row)->field();
    removeOrderItem(row);
    addAvailableField(field);
    updateState();
  }
}

} // namespace chrono
} // namespace ma

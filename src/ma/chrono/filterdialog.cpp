/*
TRANSLATOR ma::chrono::FilterDialog
*/

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/conditiondialog.h>
#include <ma/chrono/filterdialog.h>

namespace ma
{
  namespace chrono
  {    
    FilterDialog::FilterDialog(const ResourceManagerPtr& resourceManager, 
      const TableDescription& tableDescription, 
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction,
      QWidget* parent)
      : QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint)
      , tableDescription_(tableDescription)      
    {      
      ui_.setupUi(this);
      conditionDialog_ = new ConditionDialog(resourceManager, tableDescription, taskWindowManager, databaseModel, helpAction, this);
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      updateState();
    }

    FilterDialog::~FilterDialog()
    {
    }       

    void FilterDialog::setFilterExpression(const FilterExpression& filterExpression)
    {
      typedef FilterExpression::ItemList::const_iterator iterator;

      filterExpression_.clear();
      ui_.conditionList->clear();

      FilterExpression::ItemList filterItems = filterExpression.items();
      for (iterator i = filterItems.begin(), end = filterItems.end(); i != end; ++i)
      {
        addFilterExpressionItem(*i);
      }
    }

    FilterExpression FilterDialog::filterExpression() const
    {
      return filterExpression_;
    }
    
    void FilterDialog::updateState()
    {      
      ui_.removeBtn->setEnabled(ui_.conditionList->currentRow() >= 0 && ui_.conditionList->count() > 0);            
    }

    void FilterDialog::addFilterExpressionItem(const FilterExpression::ItemPtr& item)
    {
      typedef QVector<int>::const_iterator iterator;                  
      filterExpression_.addItem(item);
      ui_.conditionList->addItem(item->ñonditionText());
    }    

    void FilterDialog::on_addBtn_clicked(bool /*checked*/)
    {
      if (QDialog::Accepted == conditionDialog_->exec())
      {        
        FilterExpression::ItemPtr filterItem = conditionDialog_->condition();
        if (filterItem)
        {                    
          addFilterExpressionItem(filterItem);
          updateState();
        }                
      }      
    }    

    void FilterDialog::on_removeBtn_clicked(bool /*checked*/)
    {
      int itemIndex = ui_.conditionList->currentRow();
      if (itemIndex >= 0)
      {
        filterExpression_.removeItem(itemIndex);
        std::auto_ptr<QListWidgetItem> item(ui_.conditionList->takeItem(itemIndex));
        updateState();
      }
    }

    void FilterDialog::on_conditionList_itemSelectionChanged()
    {
      updateState();
    }

  } // namespace chrono
} // namespace ma
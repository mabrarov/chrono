//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_AUTOENTERPRISELISTPAGE_H
#define MA_CHRONO_AUTOENTERPRISELISTPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/autoenterprisetablemodel_fwd.h>
#include <ma/chrono/model/autoenterprise_fwd.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/listpage.h>

namespace ma {
namespace chrono {

class AutoenterpriseListPage : public ListPage
{
  Q_OBJECT

public:
  AutoenterpriseListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, 
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent);
  ~AutoenterpriseListPage();

  void refresh();
  std::auto_ptr<ListPage> clone(QWidget* parent = 0) const;
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;
  OptionalQString relationalTextFromRow(int row) const;

protected:
  void applyUserDefinedFilter(const UserDefinedFilter& userDefinedFilter);
  void applyUserDefinedOrder(const UserDefinedOrder& userDefinedOrder);

private:
  Q_DISABLE_COPY(AutoenterpriseListPage)                      

  AutoenterpriseTableModel* model_;

private slots:
  void on_database_connectionStateChanged();
  void on_database_aboutToClose();      
  void on_database_autoenterpriseInserted(
      const ma::chrono::model::Autoenterprise& autoenterprise);
  void on_database_autoenterpriseUpdated(
      const ma::chrono::model::Autoenterprise& autoenterprise);
  void on_database_autoenterpriseRemoved(
      const ma::chrono::model::Autoenterprise& autoenterprise);      
  void on_database_userUpdated(const ma::chrono::model::User& user);
}; // class AutoenterpriseListPage

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_AUTOENTERPRISELISTPAGE_H

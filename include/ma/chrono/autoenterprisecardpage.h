//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_AUTOENTERPRISECARDPAGE_H
#define MA_CHRONO_AUTOENTERPRISECARDPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ui_autoenterprisecardpage.h>
#include <ma/chrono/types.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/autoenterprise.h>
#include <ma/chrono/cardpage.h>

namespace ma {
namespace chrono {

class AutoenterpriseCardPage : public CardPage
{
  Q_OBJECT

public: 
  AutoenterpriseCardPage(const ResourceManagerPtr& resourceManager, 
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent = 0);
  ~AutoenterpriseCardPage();

  void refresh();
  void save();
  void cancel();
  void edit();      
  void remove();
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;
            
private:
  Q_DISABLE_COPY(AutoenterpriseCardPage)
    
  void connectDataAwareWidgets(const QWidgetList&);
  void updateWidgets();

  bool populateFromWidgets();
  bool populateFromDatabase();
  void populateWidgets();
  void populateExportData();
  void populateRelationalWidgets();
  bool updateDatabase();
  bool insertIntoDatabase();
  bool removeFromDatabase();                  
  void showRequiredMarkers(bool visible);

  Ui::autoenterpriseCardPage ui_;
  QWidgetList dataAwareWidgets_;    
  QWidgetList requiredMarkers_;
  bool        contentsChanged_;

  model::Autoenterprise entityData_;            
  QString               createUserName_;      
  QString               updateUserName_;

private slots:
  void on_lineEdit_textEdited(const QString&);
  void on_textEdit_textChanged();
  void on_createUserBtn_clicked(bool checked = false);
  void on_updateUserBtn_clicked(bool checked = false);

  void on_database_userUpdated(const ma::chrono::model::User& user);      
}; // class AutoenterpriseCardPage    

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_AUTOENTERPRISECARDPAGE_H

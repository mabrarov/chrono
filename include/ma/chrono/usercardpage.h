//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_USERCARDPAGE_H
#define MA_CHRONO_USERCARDPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ui_usercardpage.h>
#include <ma/chrono/model/user.h>
#include <ma/chrono/cardpage.h>

namespace ma {
namespace chrono {

class UserCardPage : public CardPage
{
  Q_OBJECT

public: 
  UserCardPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent = 0);
  ~UserCardPage();

  void refresh();
  void save();
  void cancel();
  void edit();      
  void remove();
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;      
            
private:
  Q_DISABLE_COPY(UserCardPage)

  void connectDataAwareWidgets();
  void updateWidgets();

  bool populateFromWidgets();
  bool populateFromDatabase();
  void populateWidgets();
  void populateExportData();
  bool updateDatabase();
  bool insertIntoDatabase();
  bool removeFromDatabase();            
  bool checkAccess(bool self = true);      
  void showRequiredMarkers(bool visible);

  Ui::userCardPage ui_;
  QWidgetList dataAwareWidgets_;    
  QWidgetList changePasswordSelectWidgets_;    
  QWidgetList passwordAwareWidgets_;    
  QWidgetList requiredMarkers_;
  bool contentsChanged_;

  model::User entityData_;      
  OptionalQString passwordHash_;
  OptionalQString createUserName_;      
  OptionalQString updateUserName_;      

private slots:
  void on_lineEdit_textEdited(const QString&);
  void on_changePasswordCheck_stateChanged(int state);            
  void on_createUserBtn_clicked(bool checked = false);
  void on_updateUserBtn_clicked(bool checked = false);

  void on_database_userUpdated(const ma::chrono::model::User& user);
}; // class UserCardPage    

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_USERCARDPAGE_H

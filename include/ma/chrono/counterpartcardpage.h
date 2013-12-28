//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_COUNTERPARTCARDPAGE_H
#define MA_CHRONO_COUNTERPARTCARDPAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ui_counterpartcardpage.h>
#include <ma/chrono/model/user_fwd.h>
#include <ma/chrono/model/country_fwd.h>
#include <ma/chrono/model/counterpart.h>
#include <ma/chrono/cardpage.h>

namespace ma {
namespace chrono {

class CounterpartCardPage : public CardPage
{
  Q_OBJECT

public: 
  CounterpartCardPage(
      const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Mode mode, 
      const OptionalQInt64& entityId, QWidget* parent = 0);
  ~CounterpartCardPage();

  void refresh();
  void save();
  void cancel();
  void edit();      
  void remove();
  std::auto_ptr<CardPage> createCardPage(CardPage::Mode mode, 
      const OptionalQInt64& entityId) const;
            
private:
  Q_DISABLE_COPY(CounterpartCardPage)

  void connectDataAwareWidgets();
  void updateWidgets();

  bool populateFromWidgets();
  bool populateFromDatabase();
  void populateWidgets();      
  void populateExportData();
  bool updateDatabase();
  bool insertIntoDatabase();
  bool removeFromDatabase();            
  void showRequiredMarkers(bool visible);

  Ui::counterpartCardPage ui_;
  QWidgetList dataAwareWidgets_;    
  QWidgetList requiredMarkers_;
  bool contentsChanged_;

  model::Counterpart entityData_;            
  QString countryFullName_;
  QString createUserName_;      
  QString updateUserName_;

  OptionalQInt64 selectedCountryId_;

private slots:
  void on_lineEdit_textEdited(const QString&);            
  void on_countryBtn_clicked(bool checked = false);
  void on_createUserBtn_clicked(bool checked = false);
  void on_updateUserBtn_clicked(bool checked = false);

  void on_database_countryUpdated(const ma::chrono::model::Country& country);
  void on_database_countryRemoved(const ma::chrono::model::Country& country);      
  void on_database_userUpdated(const ma::chrono::model::User& user);
}; // class CounterpartCardPage    

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_COUNTERPARTCARDPAGE_H

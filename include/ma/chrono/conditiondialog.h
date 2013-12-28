//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_CONDITIONDIALOG_H
#define MA_CHRONO_CONDITIONDIALOG_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <QAction>
#include <QVector>
#include <ma/chrono/types.h>
#include <ma/chrono/resourcemanager_fwd.h>
#include <ma/chrono/taskwindowmanager_fwd.h>
#include <ma/chrono/databasemodel_fwd.h>
#include <ma/chrono/basicfiltercondition.h>
#include <ma/chrono/tabledescription.h>
#include <ma/chrono/listpage_fwd.h>
#include <ma/chrono/fielddescription.h>
#include <ui_conditiondialog.h>
#include <ma/chrono/conditiondialog_fwd.h>

namespace ma {
namespace chrono {

class ConditionDialog : public QDialog
{
  Q_OBJECT

public:
  ConditionDialog(const ResourceManagerPtr& resourceManager, 
      const TableDescription& tableDescription, 
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction,
      QWidget* parent);
  ~ConditionDialog();
  boost::shared_ptr<BasicFilterCondition> condition() const;

private:
  enum PageIndexType
  {
    emptyPageIndex = 0,
    commonPageIndex = 0,
    entityPageIndex,
    datePageIndex,
    stringPageIndex,
    boolPageIndex,
    pageCount
  }; // enum PageIndexType

  typedef boost::function<
      std::auto_ptr<BasicFilterCondition> (const ConditionDialog&)> 
      ConditionBuilder;

  struct ConditionItem
  {
    QString text;
    int     pageIndex;
    ConditionBuilder builder;

    ConditionItem(const QString& the_text, int the_pageIndex, 
        ConditionBuilder the_builder)
      : text(the_text)
      , pageIndex(the_pageIndex)
      , builder(the_builder)
    {
    }

    ConditionItem()
    {
    }
  }; // struct ConditionItem

  typedef QVector<ConditionItem> ConditionItemVector;
      
  Q_DISABLE_COPY(ConditionDialog)
  void updateState();
  static void fillFieldCombo(QComboBox& combo, 
      const TableDescription& tableDescription);
  static void fillConditionCombo(QComboBox& combo, 
      const ConditionItemVector& conditionItems);
  std::auto_ptr<ListPage> createListPage(Entity entity) const;
  void selectEntity(Entity entity);
  static QVector<ConditionItemVector> createFieldConditionItems(
      const TableDescription& tableDescription);
  static ConditionItemVector createConditionItems(
      const FieldDescriptionPtr& fieldDescription);
  static ConditionItemVector createCommonConditionItems(
      const FieldDescriptionPtr& fieldDescription);
  static ConditionItemVector createEntityConditionItems(
      const FieldDescriptionPtr& fieldDescription);
  static ConditionItemVector createDateConditionItems(
      const FieldDescriptionPtr& fieldDescription);
  static ConditionItemVector createStringConditionItems(
      const FieldDescriptionPtr& fieldDescription);
  static ConditionItemVector createEntitySetConditionItems(
      const FieldDescriptionPtr& fieldDescription);
  static ConditionItemVector createBoolConditionItems(
      const FieldDescriptionPtr& fieldDescription);

  static std::auto_ptr<BasicFilterCondition> createNullCodition(
      const ConditionDialog& conditionDialog);
  static std::auto_ptr<BasicFilterCondition> createNotNullCodition(
      const ConditionDialog& conditionDialog);
  static std::auto_ptr<BasicFilterCondition> createEntityEqualCodition(
      const ConditionDialog& conditionDialog);
  static std::auto_ptr<BasicFilterCondition> createEntityNotEqualCodition(
      const ConditionDialog& conditionDialog);            
  static std::auto_ptr<BasicFilterCondition> createDateEqualCodition(
      const ConditionDialog& conditionDialog);
  static std::auto_ptr<BasicFilterCondition> createDateNotEqualCodition(
      const ConditionDialog& conditionDialog);
  static std::auto_ptr<BasicFilterCondition> createDateGreaterCodition(
      const ConditionDialog& conditionDialog);
  static std::auto_ptr<BasicFilterCondition> createDateLessCodition(
      const ConditionDialog& conditionDialog);
  static std::auto_ptr<BasicFilterCondition> createDateGreaterOrEqualCodition(
      const ConditionDialog& conditionDialog);
  static std::auto_ptr<BasicFilterCondition> createDateLessOrEqualCodition(
      const ConditionDialog& conditionDialog);
  static std::auto_ptr<BasicFilterCondition> createStringEqualCodition(
      const ConditionDialog& conditionDialog);
  static std::auto_ptr<BasicFilterCondition> createStringNotEqualCodition(
      const ConditionDialog& conditionDialog);
  static std::auto_ptr<BasicFilterCondition> createStringLikeCodition(
      const ConditionDialog& conditionDialog);
  static std::auto_ptr<BasicFilterCondition> createStringNotLikeCodition(
      const ConditionDialog& conditionDialog);
  static std::auto_ptr<BasicFilterCondition> createEntitySetContainsCodition(
      const ConditionDialog& conditionDialog);
  static std::auto_ptr<BasicFilterCondition> 
      createEntitySetNotContainsCodition(
      const ConditionDialog& conditionDialog);
  static std::auto_ptr<BasicFilterCondition> createEntitySetEmptyCodition(
      const ConditionDialog& conditionDialog);            
  static std::auto_ptr<BasicFilterCondition> createEntitySetNotEmptyCodition(
      const ConditionDialog& conditionDialog);            
  static std::auto_ptr<BasicFilterCondition> createBoolEqualCodition(
      const ConditionDialog& conditionDialog);
  static std::auto_ptr<BasicFilterCondition> createBoolNotEqualCodition(
      const ConditionDialog& conditionDialog);

  static QString boolText(bool value);

  Ui::conditionDialog ui_;             
  ResourceManagerPtr resourceManager_;
  TableDescription                   tableDescription_;
  TaskWindowManagerWeakPtr taskWindowManager_;
  DatabaseModelPtr   databaseModel_;
  QActionSharedPtr         helpAction_;
      
  boost::optional<Entity>  selectedEntityType_;
  OptionalQInt64  selectedEntityId_;
  OptionalQString selectedEntityText_;

  FieldDescriptionPtr selectedField_;
  ConditionItemVector selectedConditionItems_;
  boost::optional<ConditionItem> selectedConditionItem_;
  QVector<ConditionItemVector>   fieldConditionItems_;
                      
private slots: 
  void on_fieldCombo_activated(int index);
  void on_conditionCombo_activated(int index);
  void on_entityBtn_clicked(bool checked = false);
      
}; // class ConditionDialog

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_CONDITIONDIALOG_H

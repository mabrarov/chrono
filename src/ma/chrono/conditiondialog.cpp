/*
TRANSLATOR ma::chrono::ConditionDialog
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/bind.hpp>
#include <ma/chrono/constants.h>
#include <ma/chrono/listselectdialog.h>
#include <ma/chrono/autoenterpriselistpage.h>
#include <ma/chrono/banklistpage.h>
#include <ma/chrono/bollistpage.h>
#include <ma/chrono/ccdlistpage.h>
#include <ma/chrono/contractlistpage.h>
#include <ma/chrono/counterpartlistpage.h>
#include <ma/chrono/countrylistpage.h>
#include <ma/chrono/currencylistpage.h>
#include <ma/chrono/deppointlistpage.h>
#include <ma/chrono/icdoclistpage.h>
#include <ma/chrono/ictlistpage.h>
#include <ma/chrono/insurancelistpage.h>
#include <ma/chrono/lcreditlistpage.h>
#include <ma/chrono/paysheetlistpage.h>
#include <ma/chrono/residentlistpage.h>
#include <ma/chrono/sbollistpage.h>
#include <ma/chrono/ttermlistpage.h>
#include <ma/chrono/userlistpage.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/binaryfiltercondition.h>
#include <ma/chrono/unaryfiltercondition.h>
#include <ma/chrono/localeutility.h>
#include <ma/chrono/conditiondialog.h>

namespace ma {

namespace chrono {

namespace {

const char* translationContext = "ma::chrono::ConditionDialog";

}

ConditionDialog::ConditionDialog(const ResourceManagerPtr& resourceManager,
    const TableDescription& tableDescription,
    const TaskWindowManagerWeakPtr& taskWindowManager,
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction,
    QWidget* parent)
  : QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint)
  , resourceManager_(resourceManager)
  , tableDescription_(tableDescription)
  , taskWindowManager_(taskWindowManager)
  , databaseModel_(databaseModel)
  , helpAction_(helpAction)
  , fieldConditionItems_(createFieldConditionItems(tableDescription))
{
  ui_.setupUi(this);
  setWindowIcon(resourceManager->getIcon(chronoIconName));
  LocaleUtility::setupLocaleAwareWidget(ui_.dateEdit);
  LocaleUtility::setupLocaleAwareWidget(ui_.calendarWidget);
  fillFieldCombo(*ui_.fieldCombo, tableDescription);
  if (ui_.fieldCombo->count())
  {
    on_fieldCombo_activated(0);
  }
  else
  {
    on_fieldCombo_activated(-1);
  }
}

ConditionDialog::~ConditionDialog()
{
}

boost::shared_ptr<BasicFilterCondition> ConditionDialog::condition() const
{
  if (selectedConditionItem_)
  {
    return boost::shared_ptr<BasicFilterCondition>(
        selectedConditionItem_->builder(*this));
  }
  return boost::shared_ptr<BasicFilterCondition>();
}

void ConditionDialog::updateState()
{
  bool fieldSelected = static_cast<bool>(selectedField_);
  bool conditionSelected = static_cast<bool>(selectedConditionItem_);
  bool valueDefined = false;
  if (selectedConditionItem_)
  {
    switch (selectedConditionItem_->pageIndex)
    {
    case commonPageIndex:
      valueDefined = true;
      break;
    case entityPageIndex:
      valueDefined = static_cast<bool>(selectedEntityId_);
      break;
    case datePageIndex:
      valueDefined = true;
      break;
    case stringPageIndex:
      valueDefined = true;
      break;
    case boolPageIndex:
      valueDefined = true;
      break;
    }
  }
  bool done = fieldSelected && conditionSelected && valueDefined;
  ui_.okButton->setEnabled(done);
}

void ConditionDialog::fillFieldCombo(QComboBox& combo, 
    const TableDescription& tableDescription)
{
  typedef TableDescription::FieldList::const_iterator iterator;
  TableDescription::FieldList fields = tableDescription.fields();
  for (iterator i = fields.begin(), end = fields.end(); i != end; ++i)
  {
    combo.addItem((*i)->title());
  }
}

void ConditionDialog::fillConditionCombo(QComboBox& combo, 
    const ConditionItemVector& conditionItems)
{
  typedef ConditionItemVector::const_iterator iterator;
  for (iterator i = conditionItems.begin(), end = conditionItems.end(); 
      i != end; ++i)
  {
    combo.addItem(i->text);
  }
}

std::auto_ptr<ListPage> ConditionDialog::createListPage(Entity entity) const
{
  switch (entity)
  {
  case autoenterpriseEntity:
    return std::auto_ptr<ListPage>(new AutoenterpriseListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, UserDefinedFilter(), 
        UserDefinedOrder(), 0));
    break;
  case bankEntity:
    return std::auto_ptr<ListPage>(new BankListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, UserDefinedFilter(), 
        UserDefinedOrder(), 0));
    break;
  case bolEntity:
    return std::auto_ptr<ListPage>(new BolListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, OptionalInt(), 
        UserDefinedFilter(), UserDefinedOrder(), 0));
    break;
  case ccdEntity:
    return std::auto_ptr<ListPage>(new CcdListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, OptionalInt(), 
        UserDefinedFilter(), UserDefinedOrder(), 0));
    break;
  case contractEntity:
    return std::auto_ptr<ListPage>(new ContractListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, OptionalInt(), 
        UserDefinedFilter(), UserDefinedOrder(), 0));
    break;
  case counterpartEntity:
    return std::auto_ptr<ListPage>(new CounterpartListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, UserDefinedFilter(), 
        UserDefinedOrder(), 0));
    break;
  case countryEntity:
    return std::auto_ptr<ListPage>(new CountryListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, UserDefinedFilter(),
        UserDefinedOrder(), 0));
    break;
  case currencyEntity:
    return std::auto_ptr<ListPage>(new CurrencyListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, UserDefinedFilter(), 
        UserDefinedOrder(), 0));
    break;
  case deppointEntity:
    return std::auto_ptr<ListPage>(new DeppointListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, UserDefinedFilter(), 
        UserDefinedOrder(), 0));
    break;
  case icdocEntity:
    return std::auto_ptr<ListPage>(new IcdocListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, OptionalInt(), 
        UserDefinedFilter(), UserDefinedOrder(), 0));
    break;
  case ictEntity:
    return std::auto_ptr<ListPage>(new IctListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, OptionalInt(), 
        UserDefinedFilter(), UserDefinedOrder(), 0));
    break;
  case insuranceEntity:
    return std::auto_ptr<ListPage>(new InsuranceListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, OptionalInt(),
        UserDefinedFilter(), UserDefinedOrder(), 0));
    break;
  case lcreditEntity:
    return std::auto_ptr<ListPage>(new LcreditListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, OptionalInt(), 
        UserDefinedFilter(), UserDefinedOrder(), 0));
    break;
  case paysheetEntity:
    return std::auto_ptr<ListPage>(new PaysheetListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, OptionalInt(),
        UserDefinedFilter(), UserDefinedOrder(), 0));
    break;
  case residentEntity:
    return std::auto_ptr<ListPage>(new ResidentListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, UserDefinedFilter(),
        UserDefinedOrder(), 0));
    break;
  case sbolEntity:
    return std::auto_ptr<ListPage>(new SbolListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, OptionalInt(), 
        UserDefinedFilter(), UserDefinedOrder(), 0));
    break;
  case ttermEntity:
    return std::auto_ptr<ListPage>(new TtermListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, UserDefinedFilter(), 
        UserDefinedOrder(), 0));
    break;
  case userEntity:
    return std::auto_ptr<ListPage>(new UserListPage(resourceManager_, 
        taskWindowManager_, databaseModel_, helpAction_, UserDefinedFilter(), 
        UserDefinedOrder(), 0));
    break;
  default:
    return std::auto_ptr<ListPage>();
    break;
  }
}

void ConditionDialog::selectEntity(Entity entity)
{
  std::auto_ptr<ListPage> listPage = createListPage(entity);
  if (listPage.get())
  {
    ListSelectDialog listSelectDialog(this, listPage);
    listSelectDialog.setWindowModality(Qt::WindowModal);
    if (QDialog::Accepted == listSelectDialog.exec())
    {
      selectedEntityId_ = listSelectDialog.listPage()->selectedId();
      selectedEntityText_ = 
          listSelectDialog.listPage()->selectedRelationalText();
      ui_.entityEdit->setText(
          selectedEntityText_ ? selectedEntityText_.get() : QString());
      updateState();
    }
  }
}

QVector<ConditionDialog::ConditionItemVector> 
ConditionDialog::createFieldConditionItems(
    const TableDescription& tableDescription)
{
  typedef TableDescription::FieldList::const_iterator iterator;

  QVector<ConditionItemVector> fieldConditionItems;
  TableDescription::FieldList fields = tableDescription.fields();
  for (iterator i = fields.begin(), end = fields.end(); i != end; ++i)
  {
    fieldConditionItems += createConditionItems(*i);
  }
  return fieldConditionItems;
}

ConditionDialog::ConditionItemVector ConditionDialog::createConditionItems(
    const FieldDescriptionPtr& fieldDescription)
{
  ConditionItemVector conditionItems = 
      createCommonConditionItems(fieldDescription);
  switch (fieldDescription->fieldType())
  {
  case FieldDescription::entityField:
    conditionItems += createEntityConditionItems(fieldDescription);
    break;
  case FieldDescription::dateField:
    conditionItems += createDateConditionItems(fieldDescription);
    break;
  case FieldDescription::stringField:
    conditionItems += createStringConditionItems(fieldDescription);
    break;
  case FieldDescription::entitySetField:
    conditionItems += createEntitySetConditionItems(fieldDescription);
    break;
  case FieldDescription::boolField:
    conditionItems += createBoolConditionItems(fieldDescription);
    break;
  }
  return conditionItems;
}

ConditionDialog::ConditionItemVector 
ConditionDialog::createCommonConditionItems(
  const FieldDescriptionPtr& fieldDescription)
{
  if (!fieldDescription->nullable())
  {
    return ConditionItemVector();
  }
  static ConditionItemVector conditionItems;
  if (conditionItems.isEmpty())
  {
    conditionItems += ConditionItem(tr("not defined"), commonPageIndex, 
        boost::bind(&ConditionDialog::createNullCodition, _1));
    conditionItems += ConditionItem(tr("defined"), commonPageIndex, 
        boost::bind(&ConditionDialog::createNotNullCodition, _1));
  }
  return conditionItems;
}

ConditionDialog::ConditionItemVector 
ConditionDialog::createEntityConditionItems(
    const FieldDescriptionPtr& /*fieldDescription*/)
{
  static ConditionItemVector conditionItems;
  if (conditionItems.isEmpty())
  {
    conditionItems += ConditionItem(tr("equal"), entityPageIndex, 
        boost::bind(&ConditionDialog::createEntityEqualCodition, _1));
    conditionItems += ConditionItem(tr("not equal"), entityPageIndex, 
        boost::bind(&ConditionDialog::createEntityNotEqualCodition, _1));
  }
  return conditionItems;
}

ConditionDialog::ConditionItemVector 
ConditionDialog::createDateConditionItems(
    const FieldDescriptionPtr& /*fieldDescription*/)
{
  static ConditionItemVector conditionItems;
  if (conditionItems.isEmpty())
  {
    conditionItems += ConditionItem(tr("equal"), datePageIndex, 
        boost::bind(&ConditionDialog::createDateEqualCodition, _1));
    conditionItems += ConditionItem(tr("not equal"), datePageIndex, 
        boost::bind(&ConditionDialog::createDateNotEqualCodition, _1));
    conditionItems += ConditionItem(tr("greater"), datePageIndex, 
        boost::bind(&ConditionDialog::createDateGreaterCodition, _1));
    conditionItems += ConditionItem(tr("less"), datePageIndex, 
        boost::bind(&ConditionDialog::createDateLessCodition, _1));
    conditionItems += ConditionItem(tr("greater or equal"), datePageIndex, 
        boost::bind(&ConditionDialog::createDateGreaterOrEqualCodition, _1));
    conditionItems += ConditionItem(tr("less or equal"), datePageIndex, 
        boost::bind(&ConditionDialog::createDateLessOrEqualCodition, _1));
  }
  return conditionItems;
}

ConditionDialog::ConditionItemVector 
ConditionDialog::createStringConditionItems(
    const FieldDescriptionPtr& /*fieldDescription*/)
{
  static ConditionItemVector conditionItems;
  if (conditionItems.isEmpty())
  {
    conditionItems += ConditionItem(tr("equal"), stringPageIndex, 
        boost::bind(&ConditionDialog::createStringEqualCodition, _1));
    conditionItems += ConditionItem(tr("not equal"), stringPageIndex, 
        boost::bind(&ConditionDialog::createStringNotEqualCodition, _1));
    conditionItems += ConditionItem(tr("like"), stringPageIndex, 
        boost::bind(&ConditionDialog::createStringLikeCodition, _1));
    conditionItems += ConditionItem(tr("not like"), stringPageIndex, 
        boost::bind(&ConditionDialog::createStringNotLikeCodition, _1));
  }
  return conditionItems;
}

ConditionDialog::ConditionItemVector 
ConditionDialog::createBoolConditionItems(
    const FieldDescriptionPtr& /*fieldDescription*/)
{
  static ConditionItemVector conditionItems;
  if (conditionItems.isEmpty())
  {
    conditionItems += ConditionItem(tr("equal"), boolPageIndex, 
        boost::bind(&ConditionDialog::createBoolEqualCodition, _1));
    conditionItems += ConditionItem(tr("not equal"), boolPageIndex, 
        boost::bind(&ConditionDialog::createBoolNotEqualCodition, _1));
  }
  return conditionItems;
}

ConditionDialog::ConditionItemVector 
ConditionDialog::createEntitySetConditionItems(
  const FieldDescriptionPtr& /*fieldDescription*/)
{
  static ConditionItemVector conditionItems;
  if (conditionItems.isEmpty())
  {
    conditionItems += ConditionItem(tr("contains"), entityPageIndex, 
        boost::bind(&ConditionDialog::createEntitySetContainsCodition, _1));
    conditionItems += ConditionItem(tr("not contains"), entityPageIndex, 
        boost::bind(&ConditionDialog::createEntitySetNotContainsCodition, _1));
    conditionItems += ConditionItem(tr("is empty"), emptyPageIndex, 
        boost::bind(&ConditionDialog::createEntitySetEmptyCodition, _1));
    conditionItems += ConditionItem(tr("is not empty"), emptyPageIndex, 
        boost::bind(&ConditionDialog::createEntitySetNotEmptyCodition, _1));
  }
  return conditionItems;
}

std::auto_ptr<BasicFilterCondition> ConditionDialog::createNullCodition(
    const ConditionDialog& conditionDialog)
{
  return std::auto_ptr<BasicFilterCondition>(new UnaryFilterCondition(
      conditionDialog.selectedField_, UnaryFilterCondition::isNullCondition));
}

std::auto_ptr<BasicFilterCondition> ConditionDialog::createNotNullCodition(
    const ConditionDialog& conditionDialog)
{
  return std::auto_ptr<BasicFilterCondition>(new UnaryFilterCondition(
      conditionDialog.selectedField_, 
      UnaryFilterCondition::isNotNullCondition));
}

std::auto_ptr<BasicFilterCondition> ConditionDialog::createEntityEqualCodition(
    const ConditionDialog& conditionDialog)
{
  return std::auto_ptr<BasicFilterCondition>(new BinaryFilterCondition(
      conditionDialog.selectedField_, 
      BinaryFilterCondition::equalsCondition, 
      conditionDialog.selectedEntityText_.get(), 
      conditionDialog.selectedEntityId_.get()));
}

std::auto_ptr<BasicFilterCondition> 
ConditionDialog::createEntityNotEqualCodition(
    const ConditionDialog& conditionDialog)
{
  return std::auto_ptr<BasicFilterCondition>(new BinaryFilterCondition(
      conditionDialog.selectedField_,
      BinaryFilterCondition::notEqualCondition, 
      conditionDialog.selectedEntityText_.get(), 
      conditionDialog.selectedEntityId_.get()));
}

std::auto_ptr<BasicFilterCondition> ConditionDialog::createDateEqualCodition(
    const ConditionDialog& conditionDialog)
{
  QDate selectedDate = conditionDialog.ui_.dateEdit->date();
  return std::auto_ptr<BasicFilterCondition>(new BinaryFilterCondition(
      conditionDialog.selectedField_,
      BinaryFilterCondition::equalsCondition, 
      selectedDate.toString(Qt::DefaultLocaleShortDate), 
      selectedDate));
}

std::auto_ptr<BasicFilterCondition> ConditionDialog::createDateNotEqualCodition(
    const ConditionDialog& conditionDialog)
{
  QDate selectedDate = conditionDialog.ui_.dateEdit->date();
  return std::auto_ptr<BasicFilterCondition>(new BinaryFilterCondition(
      conditionDialog.selectedField_,
      BinaryFilterCondition::notEqualCondition, 
      selectedDate.toString(Qt::DefaultLocaleShortDate), 
      selectedDate));
}

std::auto_ptr<BasicFilterCondition> ConditionDialog::createDateGreaterCodition(
    const ConditionDialog& conditionDialog)
{
  QDate selectedDate = conditionDialog.ui_.dateEdit->date();
  return std::auto_ptr<BasicFilterCondition>(new BinaryFilterCondition(
      conditionDialog.selectedField_,
      BinaryFilterCondition::greaterCondition, 
      selectedDate.toString(Qt::DefaultLocaleShortDate), 
      selectedDate));
}

std::auto_ptr<BasicFilterCondition> ConditionDialog::createDateLessCodition(
    const ConditionDialog& conditionDialog)
{
  QDate selectedDate = conditionDialog.ui_.dateEdit->date();
  return std::auto_ptr<BasicFilterCondition>(new BinaryFilterCondition(
      conditionDialog.selectedField_,
      BinaryFilterCondition::lessCondition, 
      selectedDate.toString(Qt::DefaultLocaleShortDate),
      selectedDate));
}

std::auto_ptr<BasicFilterCondition> 
ConditionDialog::createDateGreaterOrEqualCodition(
    const ConditionDialog& conditionDialog)
{
  QDate selectedDate = conditionDialog.ui_.dateEdit->date();
  return std::auto_ptr<BasicFilterCondition>(new BinaryFilterCondition(
      conditionDialog.selectedField_,
      BinaryFilterCondition::greaterOrEqualCondition, 
      selectedDate.toString(Qt::DefaultLocaleShortDate), 
      selectedDate));
}

std::auto_ptr<BasicFilterCondition> 
ConditionDialog::createDateLessOrEqualCodition(
    const ConditionDialog& conditionDialog)
{
  QDate selectedDate = conditionDialog.ui_.dateEdit->date();
  return std::auto_ptr<BasicFilterCondition>(new BinaryFilterCondition(
      conditionDialog.selectedField_,
      BinaryFilterCondition::lessOrEqualCondition, 
      selectedDate.toString(Qt::DefaultLocaleShortDate),
      selectedDate));
}

std::auto_ptr<BasicFilterCondition> ConditionDialog::createStringEqualCodition(
    const ConditionDialog& conditionDialog)
{
  return std::auto_ptr<BasicFilterCondition>(new BinaryFilterCondition(
      conditionDialog.selectedField_,
      BinaryFilterCondition::equalsCondition, 
      conditionDialog.ui_.stringEdit->text(),
      conditionDialog.ui_.stringEdit->text()));
}

std::auto_ptr<BasicFilterCondition> 
ConditionDialog::createStringNotEqualCodition(
    const ConditionDialog& conditionDialog)
{
  return std::auto_ptr<BasicFilterCondition>(new BinaryFilterCondition(
      conditionDialog.selectedField_,
      BinaryFilterCondition::notEqualCondition, 
      conditionDialog.ui_.stringEdit->text(),
      conditionDialog.ui_.stringEdit->text()));
}

std::auto_ptr<BasicFilterCondition> ConditionDialog::createBoolEqualCodition(
    const ConditionDialog& conditionDialog)
{
  bool value = Qt::Checked == conditionDialog.ui_.checkBox->checkState();
  return std::auto_ptr<BasicFilterCondition>(new BinaryFilterCondition(
      conditionDialog.selectedField_,
      BinaryFilterCondition::equalsCondition, 
      boolText(value), 
      QVariant(value ? 1 : 0)));
}

std::auto_ptr<BasicFilterCondition> ConditionDialog::createBoolNotEqualCodition(
    const ConditionDialog& conditionDialog)
{
  bool value = Qt::Checked == conditionDialog.ui_.checkBox->checkState();
  return std::auto_ptr<BasicFilterCondition>(new BinaryFilterCondition(
      conditionDialog.selectedField_,
      BinaryFilterCondition::notEqualCondition, 
      boolText(value), 
      QVariant(value ? 1 : 0)));
}

QString ConditionDialog::boolText(bool value)
{
  static const char* trueText = QT_TR_NOOP("True");
  static const char* falseText = QT_TR_NOOP("False");
  if (value)
  {
    return qApp->translate(translationContext, trueText);
  }
  return qApp->translate(translationContext, falseText);
}

std::auto_ptr<BasicFilterCondition> ConditionDialog::createStringLikeCodition(
    const ConditionDialog& conditionDialog)
{
  return std::auto_ptr<BasicFilterCondition>(new BinaryFilterCondition(
      conditionDialog.selectedField_,
      BinaryFilterCondition::likeCondition, 
      conditionDialog.ui_.stringEdit->text(), 
      conditionDialog.ui_.stringEdit->text()));
}

std::auto_ptr<BasicFilterCondition> 
ConditionDialog::createStringNotLikeCodition(
    const ConditionDialog& conditionDialog)
{
  return std::auto_ptr<BasicFilterCondition>(new BinaryFilterCondition(
      conditionDialog.selectedField_,
      BinaryFilterCondition::notLikeCondition, 
      conditionDialog.ui_.stringEdit->text(), 
      conditionDialog.ui_.stringEdit->text()));
}

std::auto_ptr<BasicFilterCondition> 
ConditionDialog::createEntitySetContainsCodition(
    const ConditionDialog& conditionDialog)
{
  return std::auto_ptr<BasicFilterCondition>(new BinaryFilterCondition(
      conditionDialog.selectedField_,
      BinaryFilterCondition::containsCondition, 
      conditionDialog.selectedEntityText_.get(), 
      conditionDialog.selectedEntityId_.get()));
}

std::auto_ptr<BasicFilterCondition> 
ConditionDialog::createEntitySetNotContainsCodition(
    const ConditionDialog& conditionDialog)
{
  return std::auto_ptr<BasicFilterCondition>(new BinaryFilterCondition(
      conditionDialog.selectedField_,
      BinaryFilterCondition::notContainsCondition, 
      conditionDialog.selectedEntityText_.get(),
      conditionDialog.selectedEntityId_.get()));
}

std::auto_ptr<BasicFilterCondition> 
ConditionDialog::createEntitySetEmptyCodition(
    const ConditionDialog& conditionDialog)
{
  return std::auto_ptr<BasicFilterCondition>(new UnaryFilterCondition(
        conditionDialog.selectedField_, 
        UnaryFilterCondition::isEmptyCondition));
}

std::auto_ptr<BasicFilterCondition> 
ConditionDialog::createEntitySetNotEmptyCodition(
    const ConditionDialog& conditionDialog)
{
  return std::auto_ptr<BasicFilterCondition>(new UnaryFilterCondition(
      conditionDialog.selectedField_, 
      UnaryFilterCondition::isNotEmptyCondition));
}

void ConditionDialog::on_fieldCombo_activated(int index)
{
  if (0 > index)
  {
    selectedField_ = FieldDescriptionPtr();
    selectedConditionItems_ = ConditionItemVector();
  }
  else
  {
    selectedField_ = tableDescription_.fields().at(index);
    selectedConditionItems_ = fieldConditionItems_.at(index);
  }
  ui_.conditionCombo->clear();
  fillConditionCombo(*ui_.conditionCombo, selectedConditionItems_);
  if (ui_.conditionCombo->count())
  {
    on_conditionCombo_activated(0);
  }
  else
  {
    on_conditionCombo_activated(-1);
  }
  updateState();
}

void ConditionDialog::on_conditionCombo_activated(int index)
{
  if (0 > index)
  {
    selectedConditionItem_ = boost::optional<ConditionItem>();
  }
  else
  {
    selectedConditionItem_ = selectedConditionItems_.at(index);
  }
  if (selectedConditionItem_)
  {
    ui_.stackedWidget->setCurrentIndex(selectedConditionItem_->pageIndex);
  }
  else
  {
    ui_.stackedWidget->setCurrentIndex(emptyPageIndex);
  }
  if (selectedField_)
  {
    switch (selectedField_->fieldType())
    {
    case FieldDescription::entityField:
    case FieldDescription::entitySetField:
      if (selectedEntityType_)
      {
        if (selectedEntityType_.get() != selectedField_->entitySubType())
        {
          selectedEntityId_.reset();
          selectedEntityText_.reset();
          ui_.entityEdit->setText(QString());
        }
      }
      selectedEntityType_ = selectedField_->entitySubType();
      break;
    }
  }
  updateState();
}

void ConditionDialog::on_entityBtn_clicked(bool /*checked*/)
{
  if (selectedField_)
  {
    switch (selectedField_->fieldType())
    {
    case FieldDescription::entityField:
    case FieldDescription::entitySetField:
      selectEntity(selectedField_->entitySubType());
      break;
    }
  }
}

} // namespace chrono
} // namespace ma

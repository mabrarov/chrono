/*
TRANSLATOR ma::chrono::NavigationModel
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <memory>
#include <QStandardItem>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h> 
#include <ma/chrono/summarylistpage.h>
#include <ma/chrono/summarypage.h>
#include <ma/chrono/contractlistpage.h>
#include <ma/chrono/banklistpage.h>
#include <ma/chrono/counterpartlistpage.h>
#include <ma/chrono/insurancelistpage.h>
#include <ma/chrono/autoenterpriselistpage.h>
#include <ma/chrono/residentlistpage.h>
#include <ma/chrono/ttermlistpage.h>
#include <ma/chrono/currencylistpage.h>
#include <ma/chrono/countrylistpage.h>
#include <ma/chrono/userlistpage.h>
#include <ma/chrono/deppointlistpage.h>
#include <ma/chrono/ccdlistpage.h>
#include <ma/chrono/bollistpage.h>
#include <ma/chrono/sbollistpage.h>
#include <ma/chrono/lcreditlistpage.h>
#include <ma/chrono/paysheetlistpage.h>
#include <ma/chrono/ictlistpage.h>
#include <ma/chrono/icdoclistpage.h>
#include <ma/chrono/navigationmodel.h>
#include <ma/chrono/contracttablemodel.h>
#include <ma/chrono/insurancetablemodel.h>
#include <ma/chrono/lcredittablemodel.h>
#include <ma/chrono/boltablemodel.h>
#include <ma/chrono/sboltablemodel.h>
#include <ma/chrono/paysheettablemodel.h>
#include <ma/chrono/icttablemodel.h>
#include <ma/chrono/ccdtablemodel.h>
#include <ma/chrono/icdoctablemodel.h>

namespace ma {

namespace chrono {

NavigationModel::NavigationModel(QObject* parent, QWidget* pageParent,
    const QActionSharedPtr& helpAction, 
    const ResourceManagerPtr& resourceManager, 
    const TaskWindowManagerPtr& taskWindowManager, 
    const DatabaseModelPtr& databaseModel,
    const QSize& itemSizeHint)
  : QStandardItemModel(parent)
{
  populate(pageParent, helpAction, *this, resourceManager, taskWindowManager, databaseModel, itemSizeHint);
}

NavigationModel::~NavigationModel()
{
}

void NavigationModel::populate(QWidget* pageParent,
  const QActionSharedPtr& helpAction, 
  QStandardItemModel& navigationModel,
  const ResourceManagerPtr& resourceManager, 
  const TaskWindowManagerPtr& taskWindowManager, 
  const DatabaseModelPtr& databaseModel,
  const QSize& itemSizeHint)
{
  std::auto_ptr<QStandardItem> item(new QStandardItem(resourceManager->getIcon(homeIconName), tr("Main")));
  item->setSizeHint(itemSizeHint);
  populateMain(pageParent, helpAction, *item, resourceManager, taskWindowManager, databaseModel, itemSizeHint);
  navigationModel.appendRow(item.release());

  item.reset(new QStandardItem(resourceManager->getIcon(bookmarksOrganizeIconName), tr("References")));
  item->setSizeHint(itemSizeHint);
  populateReferences(pageParent, helpAction, *item, resourceManager, taskWindowManager, databaseModel, itemSizeHint);
  navigationModel.appendRow(item.release());

  item.reset(new QStandardItem(resourceManager->getIcon(serviceIconName), tr("Service")));
  item->setSizeHint(itemSizeHint);
  populateService(pageParent, helpAction, *item, resourceManager, taskWindowManager, databaseModel, itemSizeHint);
  navigationModel.appendRow(item.release());      
}

void NavigationModel::populateMain(QWidget* pageParent, 
  const QActionSharedPtr& helpAction,
  QStandardItem& parentItem, 
  const ResourceManagerPtr& resourceManager, 
  const TaskWindowManagerPtr& taskWindowManager, 
  const DatabaseModelPtr& databaseModel,
  const QSize& itemSizeHint)
{
  QIcon summaryIcon(resourceManager->getIcon(statisticsIconName));
  QIcon listIcon(resourceManager->getIcon(listIconName));
  QIcon filterIcon(resourceManager->getIcon(filterIconName));      

  int row = 0;            
  //std::auto_ptr<Page> page(new SummaryPage(resourceManager, taskWindowManager, databaseModel, helpAction, pageParent));
  //page->setWindowIcon(pageParent->windowIcon());      
  //page->setVisible(false);      
  //std::auto_ptr<QStandardItem> item(new QStandardItem(summaryIcon, tr("Summary")));
  //page->setWindowTitle(item->text());
  //item->setSizeHint(itemSizeHint);
  //item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);
  //parentItem.setChild(row++, item.release());
  //page.release();
            
  //page.reset(new ContractListPage(resourceManager, taskWindowManager, databaseModel, helpAction, pageParent));
  std::auto_ptr<Page> page(new ContractListPage(resourceManager, taskWindowManager, databaseModel, helpAction, OptionalInt(), UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setWindowIcon(pageParent->windowIcon());
  page->setVisible(false);      
  //item.reset(new QStandardItem(listIcon, tr("Contracts")));
  std::auto_ptr<QStandardItem> item(new QStandardItem(listIcon, tr("Contracts")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);      
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);      
  {
    int subRow = 0;
    std::auto_ptr<Page> subPage(new ContractListPage(resourceManager, taskWindowManager, databaseModel, helpAction, 
      ContractTableModel::expiredFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);      
    std::auto_ptr<QStandardItem> subItem(new QStandardItem(filterIcon, tr("Expired")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);      
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new ContractListPage(resourceManager, taskWindowManager, databaseModel, helpAction, 
      ContractTableModel::expireTodayFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Expires today")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new ContractListPage(resourceManager, taskWindowManager, databaseModel, helpAction, 
      ContractTableModel::expireTomorrowFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Expires tomorrow")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new ContractListPage(resourceManager, taskWindowManager, databaseModel, helpAction, ContractTableModel::expireIn10DaysFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Expires in 10 days")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new ContractListPage(resourceManager, taskWindowManager, databaseModel, helpAction, ContractTableModel::expireThisWeekFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Expires this week")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();
        
    subPage.reset(new ContractListPage(resourceManager, taskWindowManager, databaseModel, helpAction, ContractTableModel::expireThisMonthFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Expires this month")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new ContractListPage(resourceManager, taskWindowManager, databaseModel, helpAction, ContractTableModel::notClosedFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Not closed")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();        

    subPage.reset(new ContractListPage(resourceManager, taskWindowManager, databaseModel, helpAction, ContractTableModel::closedFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Closed")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();        
  }
  parentItem.setChild(row++, item.release());            
  page.release();

  page.reset(new InsuranceListPage(resourceManager, taskWindowManager, databaseModel, helpAction, OptionalInt(), UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setWindowIcon(pageParent->windowIcon());
  page->setVisible(false);      
  item.reset(new QStandardItem(listIcon, tr("Insurances")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);      
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);
  {
    int subRow = 0;
    std::auto_ptr<Page> subPage(new InsuranceListPage(resourceManager, taskWindowManager, databaseModel, helpAction, InsuranceTableModel::expiredFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);      
    std::auto_ptr<QStandardItem> subItem(new QStandardItem(filterIcon, tr("Expired")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);      
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new InsuranceListPage(resourceManager, taskWindowManager, databaseModel, helpAction, InsuranceTableModel::expireTodayFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Expires today")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new InsuranceListPage(resourceManager, taskWindowManager, databaseModel, helpAction, InsuranceTableModel::expireTomorrowFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Expires tomorrow")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new InsuranceListPage(resourceManager, taskWindowManager, databaseModel, helpAction, InsuranceTableModel::expireIn10DaysFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Expires in 10 days")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new InsuranceListPage(resourceManager, taskWindowManager, databaseModel, helpAction, InsuranceTableModel::expireThisWeekFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Expires this week")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new InsuranceListPage(resourceManager, taskWindowManager, databaseModel, helpAction, InsuranceTableModel::expireThisMonthFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Expires this month")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();        
  }
  parentItem.setChild(row++, item.release());            
  page.release();      

  page.reset(new CcdListPage(resourceManager, taskWindowManager, databaseModel, helpAction, OptionalInt(), UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setWindowIcon(pageParent->windowIcon());
  page->setVisible(false);      
  item.reset(new QStandardItem(listIcon, tr("Cargo Customs Declarations")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);      
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);
  {
    int subRow = 0;
    std::auto_ptr<Page> subPage(new CcdListPage(resourceManager, taskWindowManager, databaseModel, helpAction, CcdTableModel::notHasIcdocFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);      
    std::auto_ptr<QStandardItem> subItem(new QStandardItem(filterIcon, tr("No inquiry on confirming documents")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);              
  //  {
  //    int subRow = 0;
  //    std::auto_ptr<Page> subPage2(new CcdListPage(resourceManager, taskWindowManager, databaseModel, helpAction, pageParent));
  //    subPage2->setWindowIcon(pageParent->windowIcon());
  //    subPage2->setVisible(false);      
  //    std::auto_ptr<QStandardItem> subItem2(new QStandardItem(filterIcon, tr("Inquiry expired")));
  //    subPage2->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem2->text()));
  //    subItem2->setSizeHint(itemSizeHint);        
  //    subItem2->setData(qVariantFromValue(static_cast<QObject*>(subPage2.get())), pageItemDataRole);              
  //    subItem->setChild(subRow++, subItem2.release());      
  //    subPage2.release();

  //    subPage2.reset(new CcdListPage(resourceManager, taskWindowManager, databaseModel, helpAction, pageParent));
  //    subPage2->setWindowIcon(pageParent->windowIcon());
  //    subPage2->setVisible(false);      
  //    subItem2.reset(new QStandardItem(filterIcon, tr("Inquiry expires today")));
  //    subPage2->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem2->text()));
  //    subItem2->setSizeHint(itemSizeHint);        
  //    subItem2->setData(qVariantFromValue(static_cast<QObject*>(subPage2.get())), pageItemDataRole);              
  //    subItem->setChild(subRow++, subItem2.release());      
  //    subPage2.release();

  //    subPage2.reset(new CcdListPage(resourceManager, taskWindowManager, databaseModel, helpAction, pageParent));
  //    subPage2->setWindowIcon(pageParent->windowIcon());
  //    subPage2->setVisible(false);      
  //    subItem2.reset(new QStandardItem(filterIcon, tr("Inquiry expires tomorrow")));
  //    subPage2->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem2->text()));
  //    subItem2->setSizeHint(itemSizeHint);        
  //    subItem2->setData(qVariantFromValue(static_cast<QObject*>(subPage2.get())), pageItemDataRole);              
  //    subItem->setChild(subRow++, subItem2.release());      
  //    subPage2.release();

  //    subPage2.reset(new CcdListPage(resourceManager, taskWindowManager, databaseModel, helpAction, pageParent));
  //    subPage2->setWindowIcon(pageParent->windowIcon());
  //    subPage2->setVisible(false);      
  //    subItem2.reset(new QStandardItem(filterIcon, tr("Inquiry expires this week")));
  //    subPage2->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem2->text()));
  //    subItem2->setSizeHint(itemSizeHint);        
  //    subItem2->setData(qVariantFromValue(static_cast<QObject*>(subPage2.get())), pageItemDataRole);              
  //    subItem->setChild(subRow++, subItem2.release());      
  //    subPage2.release();          
  //  }        
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new CcdListPage(resourceManager, taskWindowManager, databaseModel, helpAction, CcdTableModel::hasIcdocFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);      
    subItem.reset(new QStandardItem(filterIcon, tr("Has inquiry on currency transactions")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
  //  {
  //    int subRow = 0;
  //    std::auto_ptr<Page> subPage2(new CcdListPage(resourceManager, taskWindowManager, databaseModel, helpAction, pageParent));
  //    subPage2->setWindowIcon(pageParent->windowIcon());
  //    subPage2->setVisible(false);      
  //    std::auto_ptr<QStandardItem> subItem2(new QStandardItem(filterIcon, tr("Inquiry not acknowledged")));
  //    subPage2->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem2->text()));
  //    subItem2->setSizeHint(itemSizeHint);        
  //    subItem2->setData(qVariantFromValue(static_cast<QObject*>(subPage2.get())), pageItemDataRole);              
  //    subItem->setChild(subRow++, subItem2.release());      
  //    subPage2.release();

  //    subPage2.reset(new CcdListPage(resourceManager, taskWindowManager, databaseModel, helpAction, pageParent));
  //    subPage2->setWindowIcon(pageParent->windowIcon());
  //    subPage2->setVisible(false);      
  //    subItem2.reset(new QStandardItem(filterIcon, tr("Inquiry acknowledged")));
  //    subPage2->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem2->text()));
  //    subItem2->setSizeHint(itemSizeHint);        
  //    subItem2->setData(qVariantFromValue(static_cast<QObject*>(subPage2.get())), pageItemDataRole);              
  //    subItem->setChild(subRow++, subItem2.release());      
  //    subPage2.release();                   
  //  }
    item->setChild(subRow++, subItem.release());
    subPage.release();

    subPage.reset(new CcdListPage(resourceManager, taskWindowManager, databaseModel, helpAction, CcdTableModel::paymentControlExpired, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);      
    subItem.reset(new QStandardItem(filterIcon, tr("Payment control expired")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());
    subPage.release();

    subPage.reset(new CcdListPage(resourceManager, taskWindowManager, databaseModel, helpAction, CcdTableModel::zeroTaxControlExpired, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);      
    subItem.reset(new QStandardItem(filterIcon, tr("Zero tax control expired")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());
    subPage.release();
  }      
  parentItem.setChild(row++, item.release());       
  page.release();

  page.reset(new PaysheetListPage(resourceManager, taskWindowManager, databaseModel, helpAction, OptionalInt(), UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setWindowIcon(pageParent->windowIcon());
  page->setVisible(false);      
  item.reset(new QStandardItem(listIcon, tr("Pay sheets")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);      
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);      
  {
    int subRow = 0;
    std::auto_ptr<Page> subPage(new PaysheetListPage(resourceManager, taskWindowManager, databaseModel, helpAction, PaysheetTableModel::notHasIctFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);      
    std::auto_ptr<QStandardItem> subItem(new QStandardItem(filterIcon, tr("No inquiry on currency transactions")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);              
  //  {
  //    int subRow = 0;
  //    std::auto_ptr<Page> subPage2(new ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, paySheetEntity, pageParent));
  //    subPage2->setWindowIcon(pageParent->windowIcon());
  //    subPage2->setVisible(false);      
  //    std::auto_ptr<QStandardItem> subItem2(new QStandardItem(filterIcon, tr("Inquiry expired")));
  //    subPage2->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem2->text()));
  //    subItem2->setSizeHint(itemSizeHint);        
  //    subItem2->setData(qVariantFromValue(static_cast<QObject*>(subPage2.get())), pageItemDataRole);              
  //    subItem->setChild(subRow++, subItem2.release());      
  //    subPage2.release();

  //    subPage2.reset(new ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, paySheetEntity, pageParent));
  //    subPage2->setWindowIcon(pageParent->windowIcon());
  //    subPage2->setVisible(false);      
  //    subItem2.reset(new QStandardItem(filterIcon, tr("Inquiry expires today")));
  //    subPage2->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem2->text()));
  //    subItem2->setSizeHint(itemSizeHint);        
  //    subItem2->setData(qVariantFromValue(static_cast<QObject*>(subPage2.get())), pageItemDataRole);              
  //    subItem->setChild(subRow++, subItem2.release());      
  //    subPage2.release();

  //    subPage2.reset(new ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, paySheetEntity, pageParent));
  //    subPage2->setWindowIcon(pageParent->windowIcon());
  //    subPage2->setVisible(false);      
  //    subItem2.reset(new QStandardItem(filterIcon, tr("Inquiry expires tomorrow")));
  //    subPage2->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem2->text()));
  //    subItem2->setSizeHint(itemSizeHint);        
  //    subItem2->setData(qVariantFromValue(static_cast<QObject*>(subPage2.get())), pageItemDataRole);              
  //    subItem->setChild(subRow++, subItem2.release());      
  //    subPage2.release();

  //    subPage2.reset(new ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, paySheetEntity, pageParent));
  //    subPage2->setWindowIcon(pageParent->windowIcon());
  //    subPage2->setVisible(false);      
  //    subItem2.reset(new QStandardItem(filterIcon, tr("Inquiry expires this week")));
  //    subPage2->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem2->text()));
  //    subItem2->setSizeHint(itemSizeHint);        
  //    subItem2->setData(qVariantFromValue(static_cast<QObject*>(subPage2.get())), pageItemDataRole);              
  //    subItem->setChild(subRow++, subItem2.release());      
  //    subPage2.release();          
  //  }        
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new PaysheetListPage(resourceManager, taskWindowManager, databaseModel, helpAction, PaysheetTableModel::hasIctFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);      
    subItem.reset(new QStandardItem(filterIcon, tr("Has inquiry on currency transactions")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);        
  //  {
  //    int subRow = 0;
  //    std::auto_ptr<Page> subPage2(new ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, paySheetEntity, pageParent));
  //    subPage2->setWindowIcon(pageParent->windowIcon());
  //    subPage2->setVisible(false);      
  //    std::auto_ptr<QStandardItem> subItem2(new QStandardItem(filterIcon, tr("Inquiry not acknowledged")));
  //    subPage2->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem2->text()));
  //    subItem2->setSizeHint(itemSizeHint);        
  //    subItem2->setData(qVariantFromValue(static_cast<QObject*>(subPage2.get())), pageItemDataRole);              
  //    subItem->setChild(subRow++, subItem2.release());      
  //    subPage2.release();

  //    subPage2.reset(new ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, paySheetEntity, pageParent));
  //    subPage2->setWindowIcon(pageParent->windowIcon());
  //    subPage2->setVisible(false);      
  //    subItem2.reset(new QStandardItem(filterIcon, tr("Inquiry acknowledged")));
  //    subPage2->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem2->text()));
  //    subItem2->setSizeHint(itemSizeHint);        
  //    subItem2->setData(qVariantFromValue(static_cast<QObject*>(subPage2.get())), pageItemDataRole);              
  //    subItem->setChild(subRow++, subItem2.release());      
  //    subPage2.release();                   
  //  }
    item->setChild(subRow++, subItem.release());      
    subPage.release();
  }
  parentItem.setChild(row++, item.release());   
  page.release();

  page.reset(new BolListPage(resourceManager, taskWindowManager, databaseModel, helpAction, OptionalInt(), UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setWindowIcon(pageParent->windowIcon());
  page->setVisible(false);      
  item.reset(new QStandardItem(listIcon, tr("Bills of Lading")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);      
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);
  {
    int subRow = 0;
    std::auto_ptr<Page> subPage(new BolListPage(resourceManager, taskWindowManager, databaseModel, helpAction, BolTableModel::todayFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);      
    std::auto_ptr<QStandardItem> subItem(new QStandardItem(filterIcon, tr("Today dated")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);      
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new BolListPage(resourceManager, taskWindowManager, databaseModel, helpAction, BolTableModel::yesterdayFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Yesterday dated")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new BolListPage(resourceManager, taskWindowManager, databaseModel, helpAction, BolTableModel::thisWeekFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("This week dated")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new BolListPage(resourceManager, taskWindowManager, databaseModel, helpAction, BolTableModel::prevWeekFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Previous week dated")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new BolListPage(resourceManager, taskWindowManager, databaseModel, helpAction, BolTableModel::thisMonthFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("This month dated")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new BolListPage(resourceManager, taskWindowManager, databaseModel, helpAction, BolTableModel::prevMonthFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Previous month dated")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();        
  }
  parentItem.setChild(row++, item.release());  
  page.release();

  page.reset(new SbolListPage(resourceManager, taskWindowManager, databaseModel, helpAction, OptionalInt(), UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setWindowIcon(pageParent->windowIcon());
  page->setVisible(false);      
  item.reset(new QStandardItem(listIcon, tr("Shipped Bills of Lading")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);      
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);      
  {
    int subRow = 0;
    std::auto_ptr<Page> subPage(new SbolListPage(resourceManager, taskWindowManager, databaseModel, helpAction, SbolTableModel::todayFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);      
    std::auto_ptr<QStandardItem> subItem(new QStandardItem(filterIcon, tr("Today dated")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);      
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new SbolListPage(resourceManager, taskWindowManager, databaseModel, helpAction, SbolTableModel::yesterdayFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Yesterday dated")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new SbolListPage(resourceManager, taskWindowManager, databaseModel, helpAction, SbolTableModel::thisWeekFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("This week dated")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new SbolListPage(resourceManager, taskWindowManager, databaseModel, helpAction, SbolTableModel::prevWeekFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Previous week dated")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new SbolListPage(resourceManager, taskWindowManager, databaseModel, helpAction, SbolTableModel::thisMonthFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("This month dated")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new SbolListPage(resourceManager, taskWindowManager, databaseModel, helpAction, SbolTableModel::prevMonthFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Previous month dated")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();        
  }
  parentItem.setChild(row++, item.release()); 
  page.release();

  page.reset(new LcreditListPage(resourceManager, taskWindowManager, databaseModel, helpAction, OptionalInt(), UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setWindowIcon(pageParent->windowIcon());
  page->setVisible(false);      
  item.reset(new QStandardItem(listIcon, tr("Letters of credit")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);      
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);      
  {
    int subRow = 0;
    std::auto_ptr<Page> subPage(new LcreditListPage(resourceManager, taskWindowManager, databaseModel, helpAction, LcreditTableModel::expiredFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);      
    std::auto_ptr<QStandardItem> subItem(new QStandardItem(filterIcon, tr("Expired")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);      
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new LcreditListPage(resourceManager, taskWindowManager, databaseModel, helpAction, LcreditTableModel::expireTodayFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Expires today")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new LcreditListPage(resourceManager, taskWindowManager, databaseModel, helpAction, LcreditTableModel::expireTomorrowFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Expires tomorrow")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new LcreditListPage(resourceManager, taskWindowManager, databaseModel, helpAction, LcreditTableModel::expireIn10DaysFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Expires in 10 days")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new LcreditListPage(resourceManager, taskWindowManager, databaseModel, helpAction, LcreditTableModel::expireThisWeekFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Expires this week")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new LcreditListPage(resourceManager, taskWindowManager, databaseModel, helpAction, LcreditTableModel::expireThisMonthFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Expires this month")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();        
  }
  parentItem.setChild(row++, item.release()); 
  page.release();      

  page.reset(new IcdocListPage(resourceManager, taskWindowManager, databaseModel, helpAction, OptionalInt(), UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setWindowIcon(pageParent->windowIcon());
  page->setVisible(false);      
  item.reset(new QStandardItem(listIcon, tr("Inquiries on confirming documents")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);      
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);
  {
    int subRow = 0;
    std::auto_ptr<Page> subPage(new IcdocListPage(resourceManager, taskWindowManager, databaseModel, helpAction, IcdocTableModel::notConfirmedFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);      
    std::auto_ptr<QStandardItem> subItem(new QStandardItem(filterIcon, tr("Not acknowledged")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);      
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new IcdocListPage(resourceManager, taskWindowManager, databaseModel, helpAction, IcdocTableModel::confirmedFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Acknowledged")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();    
  } 
  parentItem.setChild(row++, item.release());      
  page.release();

  page.reset(new IctListPage(resourceManager, taskWindowManager, databaseModel, helpAction, OptionalInt(), UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setWindowIcon(pageParent->windowIcon());
  page->setVisible(false);      
  item.reset(new QStandardItem(listIcon, tr("Inquiries on currency transactions")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);      
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);      
  {
    int subRow = 0;
    std::auto_ptr<Page> subPage(new IctListPage(resourceManager, taskWindowManager, databaseModel, helpAction, IctTableModel::notConfirmedFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);      
    std::auto_ptr<QStandardItem> subItem(new QStandardItem(filterIcon, tr("Not acknowledged")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);      
    item->setChild(subRow++, subItem.release());      
    subPage.release();

    subPage.reset(new IctListPage(resourceManager, taskWindowManager, databaseModel, helpAction, IctTableModel::confirmedFilterId, UserDefinedFilter(), UserDefinedOrder(), pageParent));
    subPage->setWindowIcon(pageParent->windowIcon());
    subPage->setVisible(false);
    subItem.reset(new QStandardItem(filterIcon, tr("Acknowledged")));
    subPage->setWindowTitle(tr("%1 - %2").arg(item->text()).arg(subItem->text()));
    subItem->setSizeHint(itemSizeHint);        
    subItem->setData(qVariantFromValue(static_cast<QObject*>(subPage.get())), pageItemDataRole);
    item->setChild(subRow++, subItem.release());      
    subPage.release();    
  }      
  parentItem.setChild(row++, item.release());
  page.release();
}

void NavigationModel::populateReferences(QWidget* pageParent, 
  const QActionSharedPtr& helpAction,
  QStandardItem& parentItem, 
  const ResourceManagerPtr& resourceManager, 
  const TaskWindowManagerPtr& taskWindowManager, 
  const DatabaseModelPtr& databaseModel,
  const QSize& itemSizeHint)
{      
  QIcon listIcon(resourceManager->getIcon(listIconName));      

  int row = 0;
      
  std::auto_ptr<Page> page(new BankListPage(resourceManager, taskWindowManager, databaseModel, helpAction, UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setVisible(false);
  std::auto_ptr<QStandardItem> item(new QStandardItem(listIcon, tr("Banks")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);
  parentItem.setChild(row++, item.release());            
  page.release();
      
  page.reset(new CounterpartListPage(resourceManager, taskWindowManager, databaseModel, helpAction, UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setVisible(false);      
  item.reset(new QStandardItem(listIcon, tr("Counterparts")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);
  parentItem.setChild(row++, item.release());            
  page.release();

  page.reset(new AutoenterpriseListPage(resourceManager, taskWindowManager, databaseModel, helpAction, UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setVisible(false);      
  item.reset(new QStandardItem(listIcon, tr("Autoenterprises")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);
  parentItem.setChild(row++, item.release());            
  page.release();            
      
  page.reset(new ResidentListPage(resourceManager, taskWindowManager, databaseModel, helpAction, UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setVisible(false);      
  item.reset(new QStandardItem(listIcon, tr("Residents")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);
  parentItem.setChild(row++, item.release());            
  page.release();

  page.reset(new TtermListPage(resourceManager, taskWindowManager, databaseModel, helpAction, UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setVisible(false);      
  item.reset(new QStandardItem(listIcon, tr("Transaction terms")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);
  parentItem.setChild(row++, item.release());            
  page.release();

  page.reset(new DeppointListPage(resourceManager, taskWindowManager, databaseModel, helpAction, UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setVisible(false);      
  item.reset(new QStandardItem(listIcon, tr("Departure points")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);
  parentItem.setChild(row++, item.release());            
  page.release();

  page.reset(new CurrencyListPage(resourceManager, taskWindowManager, databaseModel, helpAction, UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setVisible(false);      
  item.reset(new QStandardItem(listIcon, tr("Currency")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);
  parentItem.setChild(row++, item.release());            
  page.release();           

  page.reset(new CountryListPage(resourceManager, taskWindowManager, databaseModel, helpAction, UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setVisible(false);      
  item.reset(new QStandardItem(listIcon, tr("Countries")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);
  parentItem.setChild(row++, item.release());            
  page.release();                      
}

void NavigationModel::populateService(QWidget* pageParent, 
  const QActionSharedPtr& helpAction,
  QStandardItem& parentItem, 
  const ResourceManagerPtr& resourceManager, 
  const TaskWindowManagerPtr& taskWindowManager, 
  const DatabaseModelPtr& databaseModel,
  const QSize& itemSizeHint)
{
  QIcon filterIcon(resourceManager->getIcon(filterIconName));

  int row = 0;

  std::auto_ptr<Page> page(new UserListPage(resourceManager, taskWindowManager, databaseModel, helpAction, UserDefinedFilter(), UserDefinedOrder(), pageParent));
  page->setVisible(false);
  std::auto_ptr<QStandardItem> item(new QStandardItem(resourceManager->getIcon(usersIconName), tr("Users")));
  page->setWindowTitle(item->text());
  item->setSizeHint(itemSizeHint);
  item->setData(qVariantFromValue(static_cast<QObject*>(page.get())), pageItemDataRole);
  parentItem.setChild(row++, item.release());            
  page.release();           
      
  item.reset(new QStandardItem(resourceManager->getIcon(databaseIconName), tr("Maintain database")));
  item->setSizeHint(itemSizeHint);            
  parentItem.setChild(row++, item.release());

  item.reset(new QStandardItem(resourceManager->getIcon(settingsIconName), tr("Preferences")));
  item->setSizeHint(itemSizeHint);            
  parentItem.setChild(row++, item.release());      
}    

} // namespace chrono
} //namespace ma

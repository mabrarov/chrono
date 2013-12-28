/*
TRANSLATOR ma::chrono::CardPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QFrame>
#include <QLabel>
#include <QTableView>
#include <QHeaderView>
#include <QDateEdit>
#include <QCalendarWidget>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/taskwindowmanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/actionutility.h>
#include <ma/chrono/cardtaskwindow.h>
#include <ma/chrono/autoenterprisecardpage.h>
#include <ma/chrono/bankcardpage.h>
#include <ma/chrono/ccdcardpage.h>
#include <ma/chrono/contractcardpage.h>
#include <ma/chrono/counterpartcardpage.h>
#include <ma/chrono/countrycardpage.h>
#include <ma/chrono/currencycardpage.h>
#include <ma/chrono/deppointcardpage.h>
#include <ma/chrono/lcreditcardpage.h>
#include <ma/chrono/paysheetcardpage.h>
#include <ma/chrono/residentcardpage.h>
#include <ma/chrono/ttermcardpage.h>
#include <ma/chrono/usercardpage.h>
#include <ma/chrono/excelexporter.h>
#include <ma/chrono/cardpage.h>

namespace ma {

namespace chrono {    

CardPage::CardPage(const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerWeakPtr& taskWindowManager,
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, Entity entity, 
    Mode mode, const OptionalQInt64& entityId, QWidget* parent)
  : DataPage(resourceManager, taskWindowManager, 
        databaseModel, helpAction, parent)
  , entity_(entity)
  , entityId_(entityId)
  , mode_(mode)
  , primaryActions_()
  , contextActions_()
  , dataAwareActions_()
{
  actionsMenu_ = new QMenu(this);

  saveAction_ = new QAction(
      /*resourceManager->getIcon(viewIconName), */tr("&Save"), this);
  QObject::connect(saveAction_, 
      SIGNAL(triggered()), SLOT(on_saveAction_triggered()));

  cancelAction_ = new QAction(
      /*resourceManager->getIcon(viewIconName), */tr("&Cancel"), this);
  QObject::connect(cancelAction_, 
      SIGNAL(triggered()), SLOT(on_cancelAction_triggered()));

  editAction_ = new QAction(
      resourceManager->getIcon(editIconName), tr("&Edit"), this);
  QObject::connect(editAction_, 
      SIGNAL(triggered()), SLOT(on_editAction_triggered()));

  createAction_ = new QAction(
      resourceManager->getIcon(createNewIconName), tr("&New"), this);
  QObject::connect(createAction_, 
      SIGNAL(triggered()), SLOT(on_createAction_triggered()));

  deleteAction_ = new QAction(
      resourceManager->getIcon(deleteIconName), tr("&Delete"), this);
  QObject::connect(deleteAction_, 
      SIGNAL(triggered()), SLOT(on_deleteAction_triggered()));

  processActionsSeparator_ = new QAction(this);
  processActionsSeparator_->setSeparator(true);      
  specialActionsSeparator_ = new QAction(this);
  specialActionsSeparator_->setSeparator(true);            

  refreshAction_ = new QAction(
      resourceManager->getIcon(refreshIconName), tr("Re&fresh"), this);
  QObject::connect(refreshAction_, 
      SIGNAL(triggered()), SLOT(on_refreshAction_triggered()));

  sendToExcelAction_ = new QAction(tr("Send to Excel"), this);   
  QObject::connect(sendToExcelAction_, 
      SIGNAL(triggered()), SLOT(on_sendToExcelAction_triggered()));

  commonActionsSeparator_ = new QAction(this);
  commonActionsSeparator_->setSeparator(true);      

  printPageAction_ = new QAction(
      resourceManager->getIcon(printIconName), tr("&Print page"), this);
  QObject::connect(printPageAction_, 
      SIGNAL(triggered()), SLOT(on_printPageAction_triggered()));
      
  actionsMenu_->addAction(saveAction_);
  actionsMenu_->addAction(cancelAction_);
  actionsMenu_->addAction(editAction_);
  actionsMenu_->addAction(createAction_);
  actionsMenu_->addAction(deleteAction_);
  actionsMenu_->addAction(processActionsSeparator_);
  actionsMenu_->addAction(specialActionsSeparator_);      
  actionsMenu_->addAction(refreshAction_);      
  actionsMenu_->addAction(sendToExcelAction_);
  //actionsMenu_->addAction(commonActionsSeparator_);
  //actionsMenu_->addAction(printPageAction_);
    
  QAction* actionsAction = new QAction(
      resourceManager->getIcon(actionIconName), tr("&Actions"), this);
  actionsAction->setMenu(actionsMenu_);

  primaryActions_ << actionsAction;
  //todo: add customizeMenu
  customizeAction_ = 0;      
  dataAwareActions_ << saveAction_ 
                    << cancelAction_ 
                    << editAction_ 
                    << createAction_ 
                    << deleteAction_;

  QObject::connect(databaseModel.get(), 
      SIGNAL(connectionStateChanged()), 
      SLOT(on_databaseModel_connectionStateChanged()));
  updateActions();
}

CardPage::~CardPage()
{
}    

void CardPage::addPrimaryActions(const QActionList& actions)
{
  primaryActions_ << actions;      
}

void CardPage::addSpecialActions(const QActionList& actions)
{
  actionsMenu_->insertActions(specialActionsSeparator_, actions);
}

std::auto_ptr<CardPage> CardPage::createCardPage(CardPage::Mode mode, 
    const OptionalQInt64& entityId) const
{
  std::auto_ptr<CardPage> cardPage(new CardPage(resourceManager(), 
      taskWindowManager(), databaseModel(), helpAction(), 
      entity_, mode, entityId));
  return cardPage;
}

void CardPage::save()
{
  setMode(viewMode);
}

void CardPage::cancel()
{
  if (editMode == mode_)
  {
    setMode(viewMode);        
  }
  else if (createMode == mode_)
  {
    emit needToClose();
  }
}

void CardPage::edit()
{
  if (createMode != mode_)
  {        
    setMode(editMode);        
  }
}

void CardPage::remove()
{
  if (createMode != mode_)
  {        
    emit needToClose();
  }
}

void CardPage::showCardTaskWindow(Entity entity, qint64 entityId, 
    CardPageFactory cardPageFactory)
{
  if (TaskWindowManagerPtr taskWindowManager = this->taskWindowManager().lock())
  {                      
    boost::shared_ptr<CardTaskWindow> cardTaskWindow = 
        taskWindowManager->findCardTaskWindow(entity, entityId);
    if (cardTaskWindow)
    {
      if (cardTaskWindow->isMinimized())
      {
        cardTaskWindow->setWindowState(
            cardTaskWindow->windowState() & ~Qt::WindowMinimized 
                | Qt::WindowActive);
      }
      cardTaskWindow->show();
      cardTaskWindow->activateWindow();
      cardTaskWindow->raise();        
    }
    else
    {
      std::auto_ptr<CardPage> cardPage = cardPageFactory(resourceManager(), 
          taskWindowManager, databaseModel(), helpAction(), viewMode, entityId);
      cardTaskWindow = boost::make_shared<CardTaskWindow>(boost::ref(cardPage));
      taskWindowManager->add(cardTaskWindow);
      cardTaskWindow->show();
    }
  }
}

namespace {

std::auto_ptr<CardPage> createUserCardPage(
    const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerPtr& taskWindowManager, 
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, 
    CardPage::Mode viewMode, qint64 userId)
{
  std::auto_ptr<CardPage> cardPage(new UserCardPage(resourceManager, 
      taskWindowManager, databaseModel, helpAction, viewMode, userId));
  return cardPage;
}

std::auto_ptr<CardPage> createAutoenterpriseCardPage(
    const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerPtr& taskWindowManager, 
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, 
    CardPage::Mode viewMode, qint64 autoenterpriseId)
{
  std::auto_ptr<CardPage> cardPage(new AutoenterpriseCardPage(
      resourceManager, taskWindowManager, databaseModel, helpAction, 
      viewMode, autoenterpriseId));
  return cardPage;
}

std::auto_ptr<CardPage> createBankCardPage(
    const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerPtr& taskWindowManager, 
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, 
    CardPage::Mode viewMode, qint64 bankId)
{
  std::auto_ptr<CardPage> cardPage(new BankCardPage(
      resourceManager, taskWindowManager, databaseModel, helpAction, 
      viewMode, bankId));
  return cardPage;
}

std::auto_ptr<CardPage> createCounterpartCardPage(
    const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerPtr& taskWindowManager, 
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, 
    CardPage::Mode viewMode, qint64 counterpartId)
{
  std::auto_ptr<CardPage> cardPage(new CounterpartCardPage(
      resourceManager, taskWindowManager, databaseModel, helpAction, 
      viewMode, counterpartId));
  return cardPage;
}

std::auto_ptr<CardPage> createCountryCardPage(
    const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerPtr& taskWindowManager, 
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, 
    CardPage::Mode viewMode, qint64 countryId)
{
  std::auto_ptr<CardPage> cardPage(new CountryCardPage(
      resourceManager, taskWindowManager, databaseModel, helpAction, 
      viewMode, countryId));
  return cardPage;
}

std::auto_ptr<CardPage> createCurrencyCardPage(
    const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerPtr& taskWindowManager, 
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, 
    CardPage::Mode viewMode, qint64 currencyId)
{
  std::auto_ptr<CardPage> cardPage(new CurrencyCardPage(
      resourceManager, taskWindowManager, databaseModel, helpAction, 
      viewMode, currencyId));
  return cardPage;
}

std::auto_ptr<CardPage> createCcdCardPage(
    const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerPtr& taskWindowManager, 
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, 
    CardPage::Mode viewMode, qint64 ccdId)
{
  std::auto_ptr<CardPage> cardPage(new CcdCardPage(
      resourceManager, taskWindowManager, databaseModel, helpAction, 
      viewMode, ccdId));
  return cardPage;
}

std::auto_ptr<CardPage> createContractCardPage(
    const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerPtr& taskWindowManager, 
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, 
    CardPage::Mode viewMode, qint64 contractId)
{
  std::auto_ptr<CardPage> cardPage(new ContractCardPage(
      resourceManager, taskWindowManager, databaseModel, helpAction, 
      viewMode, contractId));
  return cardPage;
}

std::auto_ptr<CardPage> createResidentCardPage(
    const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerPtr& taskWindowManager, 
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, 
    CardPage::Mode viewMode, qint64 residentId)
{
  std::auto_ptr<CardPage> cardPage(new ResidentCardPage(
      resourceManager, taskWindowManager, databaseModel, helpAction, 
      viewMode, residentId));
  return cardPage;
}

std::auto_ptr<CardPage> createDeppointCardPage(
    const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerPtr& taskWindowManager, 
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, 
    CardPage::Mode viewMode, qint64 deppointId)
{
  std::auto_ptr<CardPage> cardPage(new DeppointCardPage(
      resourceManager, taskWindowManager, databaseModel, helpAction, 
      viewMode, deppointId));
  return cardPage;
}

std::auto_ptr<CardPage> createLcreditCardPage(
    const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerPtr& taskWindowManager, 
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, 
    CardPage::Mode viewMode, qint64 lcreditId)
{
  std::auto_ptr<CardPage> cardPage(new LcreditCardPage(
      resourceManager, taskWindowManager, databaseModel, helpAction, 
      viewMode, lcreditId));
  return cardPage;
}

std::auto_ptr<CardPage> createPaysheetCardPage(
    const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerPtr& taskWindowManager, 
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, 
    CardPage::Mode viewMode, qint64 paysheetId)
{
  std::auto_ptr<CardPage> cardPage(new PaysheetCardPage(
      resourceManager, taskWindowManager, databaseModel, helpAction, 
      viewMode, paysheetId));
  return cardPage;
}

std::auto_ptr<CardPage> createTtermCardPage(
    const ResourceManagerPtr& resourceManager,
    const TaskWindowManagerPtr& taskWindowManager, 
    const DatabaseModelPtr& databaseModel,
    const QActionSharedPtr& helpAction, 
    CardPage::Mode viewMode, qint64 ttermId)
{
  std::auto_ptr<CardPage> cardPage(new TtermCardPage(
      resourceManager, taskWindowManager, databaseModel, helpAction, 
      viewMode, ttermId));
  return cardPage;
}

} // anonymous namespace

void CardPage::showUserCardTaskWindow(qint64 userId)
{
  static CardPageFactory userCardPageFactory = 
      boost::bind(createUserCardPage, _1, _2, _3, _4, _5, _6);
  showCardTaskWindow(userEntity, userId, userCardPageFactory);
}

void CardPage::showAutoenterpriseCardTaskWindow(qint64 autoenterpriseId)
{
  static CardPageFactory autoenterpriseCardPageFactory = 
      boost::bind(createAutoenterpriseCardPage, _1, _2, _3, _4, _5, _6);
  showCardTaskWindow(autoenterpriseEntity, autoenterpriseId, 
      autoenterpriseCardPageFactory);
} 

void CardPage::showBankCardTaskWindow(qint64 bankId)
{
  static CardPageFactory bankCardPageFactory = 
      boost::bind(createBankCardPage, _1, _2, _3, _4, _5, _6);
  showCardTaskWindow(bankEntity, bankId, bankCardPageFactory);
}

void CardPage::showCounterpartCardTaskWindow(qint64 counterpartId)
{
  static CardPageFactory counterpartCardPageFactory = 
      boost::bind(createCounterpartCardPage, _1, _2, _3, _4, _5, _6);
  showCardTaskWindow(counterpartEntity, counterpartId, 
      counterpartCardPageFactory);
}

void CardPage::showCountryCardTaskWindow(qint64 countryId)
{
  static CardPageFactory countryCardPageFactory = 
      boost::bind(createCountryCardPage, _1, _2, _3, _4, _5, _6);
  showCardTaskWindow(countryEntity, countryId, countryCardPageFactory);
}

void CardPage::showCurrencyCardTaskWindow(qint64 currencyId)
{
  static CardPageFactory currencyCardPageFactory = 
      boost::bind(createCurrencyCardPage, _1, _2, _3, _4, _5, _6);
  showCardTaskWindow(currencyEntity, currencyId, currencyCardPageFactory);
}

void CardPage::showCcdCardTaskWindow(qint64 ccdId)
{
  static CardPageFactory ccdCardPageFactory = 
      boost::bind(createCcdCardPage, _1, _2, _3, _4, _5, _6);
  showCardTaskWindow(ccdEntity, ccdId, ccdCardPageFactory);
}

void CardPage::showContractCardTaskWindow(qint64 contractId)
{
  static CardPageFactory contractCardPageFactory = 
      boost::bind(createContractCardPage, _1, _2, _3, _4, _5, _6);
  showCardTaskWindow(contractEntity, contractId, contractCardPageFactory);
}  

void CardPage::showResidentCardTaskWindow(qint64 residentId)
{
  static CardPageFactory residentCardPageFactory = 
      boost::bind(createResidentCardPage, _1, _2, _3, _4, _5, _6);
  showCardTaskWindow(residentEntity, residentId, residentCardPageFactory);
}

void CardPage::showDeppointCardTaskWindow(qint64 deppointId)
{
  static CardPageFactory deppointCardPageFactory = 
      boost::bind(createDeppointCardPage, _1, _2, _3, _4, _5, _6);
  showCardTaskWindow(deppointEntity, deppointId, deppointCardPageFactory);
} 

void CardPage::showLcreditCardTaskWindow(qint64 lcreditId)
{
  static CardPageFactory lcreditCardPageFactory = 
      boost::bind(createLcreditCardPage, _1, _2, _3, _4, _5, _6);
  showCardTaskWindow(lcreditEntity, lcreditId, lcreditCardPageFactory);
}    

void CardPage::showPaysheetCardTaskWindow(qint64 paysheetId)
{
  static CardPageFactory paysheetCardPageFactory = 
      boost::bind(createPaysheetCardPage, _1, _2, _3, _4, _5, _6);
  showCardTaskWindow(paysheetEntity, paysheetId, paysheetCardPageFactory);
}      

void CardPage::showTtermCardTaskWindow(qint64 ttermId)
{
  static CardPageFactory ttermCardPageFactory = 
      boost::bind(createTtermCardPage, _1, _2, _3, _4, _5, _6);
  showCardTaskWindow(ttermEntity, ttermId, ttermCardPageFactory);
}

void CardPage::setEntityId(const OptionalQInt64& entityId)
{
  entityId_ = entityId;
  updateActions();
}

void CardPage::setExportDataList(const ExportDataList& exportDataList)
{
  exportDataList_ = exportDataList;
}

void CardPage::setMode(Mode mode)
{
  if (mode_ != mode)
  {
    mode_ = mode;
    updateActions();  
    emit modeChanged();
  }
}    

void CardPage::updateActions()
{
  ActionUtility::enable(dataAwareActions_, databaseModel()->isOpen());
      
  bool f = databaseModel()->isOpen() && viewMode != mode_ && entity_;
  saveAction_->setEnabled(f);
  saveAction_->setVisible(f);

  cancelAction_->setEnabled(f);      
  cancelAction_->setVisible(f);

  f = databaseModel()->isOpen() && viewMode == mode_ && entity_;
  editAction_->setEnabled(f);
  editAction_->setVisible(f);

  f = databaseModel()->isOpen() && createMode != mode_ && entity_;
  deleteAction_->setEnabled(f);
  deleteAction_->setVisible(f);
      
  refreshAction_->setEnabled(f);
  refreshAction_->setVisible(f);      

  f = databaseModel()->isOpen() && viewMode == mode_ && entity_;
  sendToExcelAction_->setEnabled(f);
}

void CardPage::on_saveAction_triggered()
{
  save();
}

void CardPage::on_cancelAction_triggered()
{
  cancel();
}

void CardPage::on_editAction_triggered()
{
  edit();
}    

void CardPage::on_createAction_triggered()
{    
  if (TaskWindowManagerPtr taskWindowManager = this->taskWindowManager().lock())
  {
    std::auto_ptr<CardPage> cardPage =
        createCardPage(CardPage::createMode, OptionalQInt64());
    boost::shared_ptr<CardTaskWindow> cardTaskWindow = 
        boost::make_shared<CardTaskWindow>(boost::ref(cardPage));
    taskWindowManager->add(cardTaskWindow);
    cardTaskWindow->show();
  }
}

void CardPage::on_deleteAction_triggered()
{
  remove();
}

void CardPage::on_refreshAction_triggered()
{
  refresh();
}    

void CardPage::on_sendToExcelAction_triggered()
{
  ExcelExporter::exportDataList(exportDataList_);      
}

void CardPage::on_printPageAction_triggered()
{
  //todo
}

void CardPage::on_databaseModel_connectionStateChanged()
{
  updateActions();       
}

} // namespace chrono
} //namespace ma

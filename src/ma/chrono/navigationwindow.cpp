/*
TRANSLATOR ma::chrono::NavigationWindow 
*/

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <memory>
#include <algorithm>
#include <boost/optional.hpp>
#include <boost/make_shared.hpp>
#include <Qt>
#include <qglobal.h>
#include <QSettings>
#include <QPoint>
#include <QApplication>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QStyleOption>
#include <QAction>
#include <QToolBox>
#include <QStatusBar>
#include <QLabel>
#include <QTreeView>
#include <QMessageBox>
#include <QMenu>
#include <QKeySequence>
#include <QDesktopWidget>
#include <QIcon>
#include <QStandardItem>
#include <ma/chrono/constants.h>
#include <ma/chrono/dbconstants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/taskwindowmanager.h>
#include <ma/chrono/addressbar.h>
#include <ma/chrono/navigationbutton.h>
#include <ma/chrono/breadcrumbbar.h>
#include <ma/chrono/commandbar.h>
#include <ma/chrono/page.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/navigationmodel.h>
#include <ma/chrono/databaseconnectdialog.h>
#include <ma/chrono/navigationwindow.h>
#include <ma/chrono/listpage.h>
#include <ma/chrono/buildenvironment.h>

namespace ma {

namespace chrono {

NavigationWindow::NavigationWindow()
  : QMainWindow()
  , resourceManager_(boost::make_shared<ResourceManager>())
  , taskWindowManager_(boost::make_shared<TaskWindowManager>())
  , databaseModel_(boost::make_shared<DatabaseModel>())      
  , navigationHistory_(historyLength)      
  , navigationTrees_()
  , navigationTreeRoots_()            
  , activeNavigationIndex_()
  , activePage_(0)
{       
  setWindowIcon(resourceManager_->getIcon(chronoIconName));
  setWindowTitle(tr("Chrono"));      

  QStyleOption styleOption;
  styleOption.initFrom(this);      
  int smallIconSize = style()->pixelMetric(
      QStyle::PM_SmallIconSize, &styleOption, this);
  QSize navigationIconSize(smallIconSize, smallIconSize);      

  goBackAction_ = new QAction(
      resourceManager_->getIcon(goBackIconName), tr("Back"), this);
  goForwardAction_ = new QAction(
      resourceManager_->getIcon(goForwardIconName), tr("Forward"), this);
  refreshAction_ = new QAction(
      resourceManager_->getIcon(refreshIconName), tr("Refresh"), this);
  goBackAction_->setEnabled(navigationHistory_.hasPrevious());
  goForwardAction_->setEnabled(navigationHistory_.hasNext());
  QObject::connect(goBackAction_, 
      SIGNAL(triggered()), SLOT(on_goBackAction_triggered()));
  QObject::connect(goForwardAction_, 
      SIGNAL(triggered()), SLOT(on_goForwardAction_triggered()));      
  QObject::connect(refreshAction_, 
      SIGNAL(triggered()), SLOT(on_refreshAction_triggered()));      

  QActionSharedPtr helpAction = boost::make_shared<QAction>(
      resourceManager_->getIcon(helpIconName), tr("&Help"), 
      static_cast<QObject*>(0));
     
  //QAction* aboutQtAction = new QAction(tr("About Qt"), this);      
  //QObject::connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
  QAction* aboutChronoAction = new QAction(tr("About Chrono"), this);      
  QObject::connect(aboutChronoAction, 
      SIGNAL(triggered()), SLOT(on_aboutChronoAction_triggered()));
  helpMenu_.reset(new QMenu());
  //helpMenu->addAction(aboutQtAction);      
  helpMenu_->addAction(aboutChronoAction);
  helpAction->setMenu(helpMenu_.get());
      
  QSize navigationItemSizeHint(navigationIconSize); 
  navigationItemSizeHint.rheight() += 6 * logicalDpiY() / defaultDpiY;      
  navigationModel_ = new NavigationModel(this, this, helpAction, 
      resourceManager_, taskWindowManager_, databaseModel_, 
      navigationItemSizeHint);
  connectListPages(*navigationModel_);

  QStatusBar* statusBar = new QStatusBar(this);
  setStatusBar(statusBar);

  databaseConnectionStateLabel_ = new QLabel(statusBar);
  {
    int left, top, right, bottom;
    databaseConnectionStateLabel_->getContentsMargins(
        &left, &top, &right, &bottom);
    int horMargin = 
        10 * databaseConnectionStateLabel_->logicalDpiX() / defaultDpiX;
    databaseConnectionStateLabel_->setContentsMargins(
        horMargin, top, horMargin, bottom);
  }
  userNameLabel_ = new QLabel(statusBar);
  {
    int left, top, right, bottom;
    userNameLabel_->getContentsMargins(&left, &top, &right, &bottom);
    int horMargin = 10 * userNameLabel_->logicalDpiX() / defaultDpiX;        
    userNameLabel_->setContentsMargins(horMargin, top, horMargin, bottom);
  }            
  statusBar->addPermanentWidget(userNameLabel_);
  statusBar->addPermanentWidget(databaseConnectionStateLabel_);

  QWidget* centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);
      
  addressBar_ = new AddressBar(centralWidget);      
  addressBar_->backButton()->setDefaultAction(goBackAction_);
  addressBar_->forwardButton()->setDefaultAction(goForwardAction_);
  addressBar_->refreshButton()->setDefaultAction(refreshAction_);
  addressBar_->addressBreadCrumbBar()->setModel(navigationModel_);      
  QObject::connect(addressBar_->addressBreadCrumbBar(), 
      SIGNAL(selected(const QModelIndex&)), 
      SLOT(on_addressBreadCrumbBar_selected(const QModelIndex&)));

  commandBar_ = new CommandBar(centralWidget);  
  QSplitter* centralSplitter = new QSplitter(centralWidget);
  centralSplitter->setOrientation(Qt::Horizontal);
                     
  navigationPane_ = new QToolBox(centralSplitter);      
  QFont font(navigationPane_->font());
  font.setPointSize(10);
  navigationPane_->setFont(font);
  int minWidth = 150 * logicalDpiX() / defaultDpiX;
  int minHeight = 150 * logicalDpiY() / defaultDpiY;
  navigationPane_->setMinimumSize(QSize(minWidth, minHeight));
  navigationPane_->layout()->setSpacing(0);
  QObject::connect(navigationPane_, 
      SIGNAL(currentChanged(int)), SLOT(on_navigationPane_currentChanged(int)));
  boost::tuple<QVector<QTreeView*>, QVector<QModelIndex> > navigationWidget = 
      populateNavigationPane(this, *navigationPane_, 
          *navigationModel_, navigationIconSize);
  navigationTrees_ = navigationWidget.get<0>();
  navigationTreeRoots_ = navigationWidget.get<1>();
  centralSplitter->addWidget(navigationPane_);

  QWidget* pageWidget = new QWidget(centralSplitter);
  pageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  pageWidget->setMinimumSize(QSize(minWidth, minHeight));      
  pageLayout_ = new QVBoxLayout(pageWidget);
  pageLayout_->setSpacing(0);
  pageLayout_->setContentsMargins(0, 0, 0, 0);      
  centralSplitter->addWidget(pageWidget);

  QBoxLayout* centralLayout = new QVBoxLayout(centralWidget);
  centralLayout->setContentsMargins(0, 0, 0, 0);
  centralLayout->setSpacing(0);
  centralLayout->addWidget(addressBar_);
  centralLayout->addWidget(commandBar_);
  centralLayout->addWidget(centralSplitter);

  databaseConnectDialog_ = new DatabaseConnectDialog(resourceManager_, this);
      
  QAction* chronoAction = new QAction(
      resourceManager_->getIcon(chronoIconName), tr("&Chrono"), this);
  connectDatabaseAction_ = new QAction(tr("&Connect to database..."), this);
  QObject::connect(connectDatabaseAction_, 
      SIGNAL(triggered()), SLOT(on_connectDatabaseAction_triggered()));
  disconnectDatabaseAction_ = new QAction(
      tr("&Disconnect from database"), this);
  QObject::connect(disconnectDatabaseAction_, 
      SIGNAL(triggered()), SLOT(on_disconnectDatabaseAction_triggered()));
  QAction* exitAction = new QAction(
      resourceManager_->getIcon(applicationQuitIconName), tr("E&xit"), this);
  exitAction->setShortcut(QKeySequence(tr("Ctrl+Q")));
  QObject::connect(exitAction, SIGNAL(triggered()), SLOT(close()));
  QMenu* chronoMenu = new QMenu(this);
  chronoMenu->addAction(connectDatabaseAction_);
  chronoMenu->addAction(disconnectDatabaseAction_);

  //todo - temporary            
  //chronoMenu->addSeparator();
  //QAction* testAction = new QAction(tr("Test action"), this);
  //QObject::connect(testAction, 
  //    SIGNAL(triggered()), SLOT(on_testAction_triggered()));
  //chronoMenu->addAction(testAction);
  //testAction = new QAction(tr("Test action 2"), this);
  //QObject::connect(testAction, 
  //    SIGNAL(triggered()), SLOT(on_testAction2_triggered()));      
  //chronoMenu->addAction(testAction);
  //testAction = new QAction(tr("Test action 3"), this);
  //QObject::connect(testAction, 
  //    SIGNAL(triggered()), SLOT(on_testAction3_triggered()));      
  //chronoMenu->addAction(testAction);
  //testAction = new QAction(tr("Test action 4"), this);
  //QObject::connect(testAction, 
  //    SIGNAL(triggered()), SLOT(on_testAction4_triggered()));      
  //chronoMenu->addAction(testAction);
      
  chronoMenu->addSeparator();      
  chronoMenu->addAction(exitAction);      
  chronoAction->setMenu(chronoMenu);
  commandBar_->addCommand(chronoAction);            
  commandBar_->addCommand(helpAction.get(), CommandBar::tray, false);               

  int navigationPaneWidth = this->width() * 3 / 10;
  int contentAreaWidth = this->width() - navigationPaneWidth;
  QList<int> centralSplitterSizes;
  centralSplitterSizes << navigationPaneWidth << contentAreaWidth;
  centralSplitter->setSizes(centralSplitterSizes);

  QObject::connect(databaseModel_.get(), 
      SIGNAL(connectionStateChanged()), 
      SLOT(on_databaseModel_connectionStateChanged()));
  QObject::connect(databaseModel_.get(), 
      SIGNAL(userUpdated(const ma::chrono::model::User&)),
      SLOT(on_userUpdated(const ma::chrono::model::User&)));
                 
  setTabOrder(addressBar_, commandBar_);
  setTabOrder(commandBar_, navigationPane_);
  setTabOrder(navigationPane_, pageWidget);      

  navigationPane_->setCurrentIndex(0);

  on_databaseModel_connectionStateChanged();
  on_navigationPane_currentChanged(0);

  navigationPane_->setFocus(Qt::MouseFocusReason);      
}

NavigationWindow::~NavigationWindow()
{
}

void NavigationWindow::closeEvent(QCloseEvent* event)
{
  //todo
  //event->accept();
  //event->ignore();
  QMainWindow::closeEvent(event);
  if (event->isAccepted())
  {
    QCoreApplication::quit();
  }
}    

void NavigationWindow::connectListPages(QStandardItemModel& navigationModel)
{
  QStandardItem* topLevelItem = navigationModel.invisibleRootItem();
  QStandardItem* currentItem = topLevelItem;
  while (currentItem)
  {
    QVariant pageItemData = 
        currentItem->data(NavigationModel::pageItemDataRole);
    if (pageItemData.canConvert<QObject*>())
    {
      if (ListPage* listPage = 
          qobject_cast<ListPage*>(pageItemData.value<QObject*>()))
      {
        QObject::connect(listPage, 
            SIGNAL(doubleClicked(const QModelIndex&)), 
            SLOT(on_listPage_doubleClicked(const QModelIndex&)));
      }
    }
    if (QStandardItem* firstChildItem = currentItem->child(0))
    {
      // Go deeper if can
      currentItem = firstChildItem;
    }
    else
    {
      // Try to go to sibling item or go level up and 
      // try to go to sibling item (recursive)
      while (currentItem)
      {
        // Find sibling item
        QStandardItem* parentItem = currentItem->parent();
        if (!parentItem && (currentItem != topLevelItem))
        {
          parentItem = topLevelItem;
        }
        if (!parentItem)
        {
          // Where are at the "invisible root item" which has no sibling items
          currentItem = 0;
          continue;
        }
        if (QStandardItem* siblingItem = 
            parentItem->child(currentItem->row() + 1))
        {
          // Sibling item found
          currentItem = siblingItem;
          break;
        }
        else
        {
          // If there is no sibling item then 
          // go level up and retry (recursion starts here)
          currentItem = parentItem;
        }
      }
    }              
  }      
}

boost::tuple<QVector<QTreeView*>, QVector<QModelIndex> > 
    NavigationWindow::populateNavigationPane(QWidget* parent, 
        QToolBox& navigationPane, QStandardItemModel& navigationModel, 
        const QSize& iconSize)
{ 
  QVector<QTreeView*> navigationTrees;
  QVector<QModelIndex> navigationTreeRoots;      
  if (QStandardItem* rootItem = navigationModel.invisibleRootItem())
  {
    for (int i = 0, rowCount = rootItem->rowCount(); i != rowCount; ++i)
    {
      QStandardItem* item = rootItem->child(i);

      std::auto_ptr<QTreeView> navigationTree(new QTreeView(parent));
      navigationTree->setModel(&navigationModel);
      navigationTree->setRootIndex(item->index());
      navigationTree->setContextMenuPolicy(Qt::CustomContextMenu);          
      navigationTree->setUniformRowHeights(true);
      navigationTree->setAnimated(true);
      navigationTree->header()->setVisible(false);
      navigationTree->setIconSize(iconSize);
      navigationTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
      QModelIndex firstChildIndex(item->index().child(0, 0));
      if (firstChildIndex.isValid())
      {
        navigationTree->setCurrentIndex(firstChildIndex);
      }

      parent->QObject::connect(navigationTree.get(), 
          SIGNAL(customContextMenuRequested(const QPoint&)),
          SLOT(on_navigationTree_contextMenuRequested(const QPoint&)));
      parent->QObject::connect(navigationTree->selectionModel(), 
          SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
          SLOT(on_navigationTree_currentChanged(const QModelIndex&, const QModelIndex&)));
      navigationPane.addItem(navigationTree.get(), item->icon(), item->text());

      navigationTrees << navigationTree.get();
      navigationTree.release();
      navigationTreeRoots << item->index();
    }
  }
  return boost::make_tuple(navigationTrees, navigationTreeRoots);
}    

void NavigationWindow::on_navigationTree_contextMenuRequested(const QPoint& pos)
{
  typedef QActionList action_list;

  if (QTreeView* tree = qobject_cast<QTreeView*>(sender()))
  {
    if (QStandardItemModel* model = 
        qobject_cast<QStandardItemModel*>(tree->model()))
    {
      QModelIndex index(tree->indexAt(pos));
      QStandardItem* treeItem = model->itemFromIndex(index);
      if (treeItem)
      {      
        QVariant pageItemData = 
            treeItem->data(NavigationModel::pageItemDataRole);
        if (pageItemData.canConvert<QObject*>())
        {
          if (Page* page = qobject_cast<Page*>(pageItemData.value<QObject*>()))
          {               
            QMenu contextMenu;
            QActionList contextActions(page->contextActions());              
            if (!contextActions.empty())
            {                  
              contextMenu.addActions(contextActions);
            }
            if (!contextMenu.isEmpty())
            {
              contextMenu.exec(tree->viewport()->mapToGlobal(pos));
            }              
          }
        }
      }        
    }
  }
}    

void NavigationWindow::setActivePage(Page* page)
{
  typedef QActionList action_list;
  typedef action_list::const_iterator action_iterator;

  if (activePage_ != page)
  {          
    if (activePage_)
    {                  
      activePage_->hide();          
      if (QAction* customizeAction = activePage_->customizeAction())
      {
        commandBar_->removeCommands(customizeAction);
      }          
      commandBar_->removeCommands(activePage_->primaryActions());
      pageLayout_->removeWidget(activePage_);
    }        
    if (page)
    {          
      pageLayout_->addWidget(page);
      commandBar_->addCommands(page->primaryActions());
      if (QAction* customizeAction = page->customizeAction())
      {
        commandBar_->addCommand(customizeAction, CommandBar::tray, false);
      }
      page->show();
    }
    activePage_ = page;        
  }
}

void NavigationWindow::setActivePage(const QModelIndex& index)
{      
  if (activeNavigationIndex_ != index)
  { 
    navigationHistory_.add(index);
    goBackAction_->setEnabled(navigationHistory_.hasPrevious());
    goForwardAction_->setEnabled(navigationHistory_.hasNext());

    Page* page = 0;
    if (index.isValid())
    {
      QStandardItem* treeItem = navigationModel_->itemFromIndex(index);
      if (treeItem)
      {      
        QVariant pageItemData = 
            treeItem->data(NavigationModel::pageItemDataRole);
        if (pageItemData.canConvert<QObject*>())
        {
          page = qobject_cast<Page*>(pageItemData.value<QObject*>());
        }
      }
    }
    setActivePage(page);
  }
}    

void NavigationWindow::on_navigationTree_currentChanged(
    const QModelIndex& current, const QModelIndex& /*previous*/)
{
  setActivePage(current);
  addressBar_->addressBreadCrumbBar()->setCurrentIndex(current);
}

void NavigationWindow::on_navigationPane_currentChanged(int index)
{      
  QTreeView* tree = 0;
  QModelIndex currentIndex;
  if (0 <= index && index < navigationTrees_.size())
  {
    tree = navigationTrees_.at(index);
    currentIndex = tree->currentIndex();
  }      
  setActivePage(currentIndex);
  addressBar_->addressBreadCrumbBar()->setCurrentIndex(currentIndex);
  if (tree)
  {
    tree->setFocus(Qt::MouseFocusReason);
  }
}

void NavigationWindow::on_addressBreadCrumbBar_selected(
    const QModelIndex& index)
{         
  boost::optional<QModelIndex> rootIndex = findNavigationTreeRoot(index);
  if (rootIndex)
  {
    QTreeView* tree = navigationTrees_.at(rootIndex.get().row());
    QModelIndex currentIndex;
    if (index != rootIndex.get())
    {
      currentIndex = index;
      tree->setCurrentIndex(index);          
    }        
    else
    {
      currentIndex = tree->currentIndex();
    }
    navigationPane_->setCurrentIndex(rootIndex.get().row());
    addressBar_->addressBreadCrumbBar()->setCurrentIndex(currentIndex);
  }
  else
  {
    addressBar_->addressBreadCrumbBar()->setCurrentIndex(
        activeNavigationIndex_);
  }      
}

void NavigationWindow::on_goBackAction_triggered()
{      
  if (navigationHistory_.hasPrevious())
  {
    QModelIndex previous(*navigationHistory_.goPrevious());
    on_addressBreadCrumbBar_selected(previous);
  }
}

void NavigationWindow::on_goForwardAction_triggered()
{
  if (navigationHistory_.hasNext())
  {
    QModelIndex next(*navigationHistory_.goNext());
    on_addressBreadCrumbBar_selected(next);
  }
}    

void NavigationWindow::on_refreshAction_triggered()
{
  if (DataPage* dataPage = qobject_cast<DataPage*>(activePage_))
  {
    dataPage->refresh();
  }      
}

boost::optional<QModelIndex> NavigationWindow::findNavigationTreeRoot(
    const QModelIndex& index) const
{
  typedef QVector<QModelIndex> index_vector;
  typedef index_vector::const_iterator index_iterator;

  boost::optional<QModelIndex> rootIndex;
  index_iterator begin(navigationTreeRoots_.begin());
  index_iterator end(navigationTreeRoots_.end());

  for (QModelIndex currentIndex = index; currentIndex.isValid(); 
      currentIndex = currentIndex.parent())
  { 
    index_iterator it = std::find(begin, end, currentIndex);
    if (it != end)
    {
      rootIndex.reset(*it);
      break;
    }
  }
  return rootIndex;
}

void NavigationWindow::on_connectDatabaseAction_triggered()
{
  databaseConnectDialog_->prepare();
  if (QDialog::Accepted == databaseConnectDialog_->exec())
  {
    QString host;
    QString databaseFilename;
    QString dbUser = QString::fromWCharArray(L"SYSDBA");
    QString dbPassword = QString::fromWCharArray(L"masterkey");
    switch (databaseConnectDialog_->currentRow())
    {
    case 0:
      {
        QSettings settings;            
        host = settings.value("Main database/host", "localhost").toString();
        databaseFilename = 
            settings.value("Main database/database", "CHRONO").toString();
        dbUser = settings.value("Main database/user", dbUser).toString();
        dbPassword = 
            settings.value("Main database/password", dbPassword).toString();
      }          
      break;

    case 2:
      host = QString::fromWCharArray(L"localhost");
      //databaseFilename = QString::fromWCharArray(L"CHRONO_DB");
      databaseFilename = QCoreApplication::applicationDirPath() 
          + QString::fromWCharArray(L"/CHRONO_DB.FDB");
      break;

    case 1:
    default:
      host = QString::fromWCharArray(L"");
      databaseFilename = QCoreApplication::applicationDirPath() 
          + QString::fromWCharArray(L"/CHRONO_DB.FDB");
      break;        
    }                
    QString user = databaseConnectDialog_->login();
    QString password = databaseConnectDialog_->password();
    DatabaseModel::OpenError openError;
    databaseModel_->open(host, databaseFilename, dbUser, dbPassword, 
        user, password, openError);
    if (DatabaseModel::openSuccess != openError)
    {
      QString title = tr("Database connection error");
      QString failReason;
      switch (openError)
      {
      case DatabaseModel::databaseClientError:
        failReason = tr("Database client failed.");
        break;
      case DatabaseModel::databaseConnectError:
        failReason = tr("Failed QObject::connect to database (server).");
        break;
      case DatabaseModel::invalidUserCredentials:
        failReason = tr("Invalid user name or password.");
        break;
      default:
        failReason = tr("Unknown error.");
        break;
      }
      QMessageBox::critical(this, title, 
          tr("Failed QObject::connect to database. %1").arg(failReason));
    }
  }
}

void NavigationWindow::on_disconnectDatabaseAction_triggered()
{
  //todo
  databaseModel_->close();
}

void NavigationWindow::on_databaseModel_connectionStateChanged()
{
  bool databaseConnected = databaseModel_->isOpen();      
  connectDatabaseAction_->setEnabled(!databaseConnected);
  connectDatabaseAction_->setVisible(!databaseConnected);
  disconnectDatabaseAction_->setEnabled(databaseConnected);
  disconnectDatabaseAction_->setVisible(databaseConnected);      
  updateUserName();      
  updateDatabaseConnectionName();
}

void NavigationWindow::on_aboutChronoAction_triggered()
{
  QWidget* activeWindow = QApplication::activeWindow();
#ifdef MA_CHRONO_BUILD_ENVIRONMENT_DEMO
  QMessageBox aboutBox(QMessageBox::NoIcon, tr("Chrono"), 
      QString::fromLatin1("<center><h3>%1</h3><p>Version %2</p>"
              "<p>Copyright (c) 2010-2015 Pet project.</p></center>")
          .arg(tr("Chrono Demo")).arg(QLatin1String("0.9.9")),
      QMessageBox::Ok, activeWindow);
#else  // MA_CHRONO_BUILD_ENVIRONMENT_DEMO
  QMessageBox aboutBox(QMessageBox::NoIcon, tr("Chrono"), 
      QString::fromLatin1("<center><h3>%1</h3><p>Version %2</p>"
              "<p>Copyright (c) 2010-2015 Pet project.</p></center>")
          .arg(tr("Chrono")).arg(QLatin1String("0.9.9")),
      QMessageBox::Ok, activeWindow);
#endif // MA_CHRONO_BUILD_ENVIRONMENT_DEMO
      
  aboutBox.setWindowIcon(this->windowIcon());
  aboutBox.exec();
} 

void NavigationWindow::on_listPage_doubleClicked(const QModelIndex& /*index*/)
{
  if (ListPage* listPage = qobject_cast<ListPage*>(sender()))
  {
    listPage->viewSelected();
  }
}

void NavigationWindow::on_userUpdated(const ma::chrono::model::User& user)
{
  if (user.userId == databaseModel_->userId())
  {
    updateUserName();  
  }      
}

void NavigationWindow::updateUserName()
{
  userNameLabel_->setText(
      databaseModel_->isOpen() ? databaseModel_->userName() : QString());
}

void NavigationWindow::updateDatabaseConnectionName()
{
  if (databaseModel_->isOpen())
  {
    if (OptionalQString connectionName = 
        databaseConnectDialog_->currentConnection())
    {
      databaseConnectionStateLabel_->setText(connectionName.get());
    }
    else
    {
      databaseConnectionStateLabel_->setText(QString());
    }        
  }      
  else
  {
    databaseConnectionStateLabel_->setText(tr("Database not connected"));
  }
}

} // namespace chrono
} // namespace ma


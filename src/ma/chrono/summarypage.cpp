/*
TRANSLATOR ma::chrono::SummaryPage
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <memory>
#include <boost/ref.hpp>
#include <boost/make_shared.hpp>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QFrame>
#include <QLabel>
#include <QTableView>
#include <QHeaderView>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/taskwindowmanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/actionutility.h>
#include <ma/chrono/listtaskwindow.h>
#include <ma/chrono/summarypage.h>

namespace ma
{
  namespace chrono
  {    
    SummaryPage::SummaryPage(const ResourceManagerPtr& resourceManager,
        const TaskWindowManagerWeakPtr& taskWindowManager,
        const DatabaseModelPtr& databaseModel,
        const QActionSharedPtr& helpAction, QWidget* parent)
      : DataPage(resourceManager, taskWindowManager, databaseModel, helpAction, parent)
      , primaryActions_()      
      , contextActions_()
      , dataAwareActions_()
    {
      ui_.setupUi(this);
      QMenu* actionsMenu = new QMenu(this);            
      //QAction* openInNewWindowAction = new QAction(tr("Open in new window"), this);      
      //QObject::connect(openInNewWindowAction, SIGNAL(triggered()), SLOT(on_openInNewWindowAction_triggered()));            
      QAction* refreshAction = new QAction(resourceManager->getIcon(refreshIconName), tr("Re&fresh"), this);      
      //QObject::connect(refreshAction_, SIGNAL(triggered()), SLOT(on_refreshAction_triggered()));            
      QAction* sendToExcelAction = new QAction(tr("Send to Excel"), this);   
      //QObject::connect(sendToExcelAction_, SIGNAL(triggered()), SLOT(on_sendToExcelAction_triggered()));      
      QAction* commonActionsSeparator = new QAction(this);
      commonActionsSeparator->setSeparator(true);      
      QAction* printPageAction = new QAction(resourceManager->getIcon(printIconName), tr("&Print page"), this);
      //QObject::connect(printPageAction_, SIGNAL(triggered()), SLOT(on_printPageAction_triggered()));      
      
      //actionsMenu->addAction(viewAction_);
      //actionsMenu->addAction(editAction_);
      //actionsMenu->addAction(createAction_);
      //actionsMenu->addAction(deleteAction_);
      //actionsMenu->addAction(processActionsSeparator_);
      //actionsMenu->addAction(specialActionsSeparator_);
      //actionsMenu->addAction(openInNewWindowAction_);
      actionsMenu->addAction(refreshAction);
      //actionsMenu->addAction(clearFilterAction_);
      actionsMenu->addAction(sendToExcelAction);
      actionsMenu->addAction(commonActionsSeparator);
      actionsMenu->addAction(printPageAction);
    
      QAction* actionsAction = new QAction(resourceManager->getIcon(actionIconName), tr("&Actions"), this);
      actionsAction->setMenu(actionsMenu);

      primaryActions_ << actionsAction;
      //todo: add customizeMenu
      customizeAction_ = 0;
      //contextActions_ << openInNewWindowAction;
      //dataAwareActions_ << viewAction_ << editAction_ << createAction_ << deleteAction_;
                  
      QTableView* grid = ui_.tableWidget;
      grid->setEditTriggers(QAbstractItemView::NoEditTriggers);
      //grid->setAlternatingRowColors(true);
      grid->setSelectionBehavior(QAbstractItemView::SelectRows);
      grid->setSelectionMode(QAbstractItemView::SingleSelection);
      grid->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
      grid->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);              
      QHeaderView* header = grid->horizontalHeader();

      // Qt 5 migration
#if QT_VERSION >= 0x050000      
      header->setSectionsMovable(true);
#else
      header->setMovable(true);
#endif

      header->setContextMenuPolicy(Qt::CustomContextMenu);
      header->setHighlightSections(false);
      header->setSortIndicatorShown(true);
      header->setStretchLastSection(true);
      QObject::connect(header, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(on_headerContextMenu_requested(const QPoint&)));
      grid->verticalHeader()->setVisible(false);      
      grid->resizeColumnsToContents();
      
      QObject::connect(databaseModel.get(), SIGNAL(connectionStateChanged()), SLOT(on_databaseModel_connectionStateChanged()));      
      on_databaseModel_connectionStateChanged();
    }

    SummaryPage::~SummaryPage()
    {
    }

    QActionList SummaryPage::primaryActions() const
    {
      return primaryActions_;
    }

    QAction* SummaryPage::customizeAction() const
    {
      return customizeAction_;
    }
     
    QActionList SummaryPage::contextActions() const
    {
      return contextActions_;
    }

    //void SummaryPage::on_openInNewWindowAction_triggered()
    //{      
    //  TaskWindowManagerPtr taskWindowManager(this->taskWindowManager().lock());
    //  if (taskWindowManager)
    //  {
    //    std::auto_ptr<SummaryPage> clonedPage = 
    //        new SummaryPage(resourceManager(), taskWindowManager(), 
    //            databaseModel(), helpAction()));
    //    clonedPage->setWindowIcon(windowIcon());
    //    clonedPage->setWindowTitle(windowTitle());        
    //    boost::shared_ptr<ListTaskWindow> listTaskWindow = 
    //        boost::make_shared<ListTaskWindow>(boost::ref(clonedPage));
    //    taskWindowManager->add(listTaskWindow);      
    //    listTaskWindow->show();
    //  }            
    //}    

    void SummaryPage::on_headerContextMenu_requested(const QPoint& pos)
    {
      QHeaderView* header = qobject_cast<QHeaderView*>(sender());
      if (header)
      {        
        if (header->count() > 2)
        {
          QMenu contextMenu;        
          Qt::Orientation headerOrientation = header->orientation();
          QAbstractItemModel* model = header->model();
          bool loneVisible = header->count() - header->hiddenSectionCount() < 2;        
          for (int i = 0, count = header->count(); i != count; ++i)
          {
            int section = header->logicalIndex(i);
            QVariant sectionData = model->headerData(section, headerOrientation);
            QString sectionCaption = sectionData.value<QString>();
            QAction* sectionAction = contextMenu.addAction(sectionCaption);
            sectionAction->setCheckable(true);
            bool visible = !header->isSectionHidden(section);
            sectionAction->setChecked(visible);
            if (loneVisible && visible)
            {
              sectionAction->setEnabled(false);
            }            
          }        
          if (!contextMenu.isEmpty())
          {
            QAction* action = contextMenu.exec(header->mapToGlobal(pos));
            int index = contextMenu.actions().indexOf(action);
            if (0 <= index)
            {
              int section = header->logicalIndex(index);
              header->setSectionHidden(section, !header->isSectionHidden(section));
            }
          }
        }
      }
    }

    void SummaryPage::on_databaseModel_connectionStateChanged()
    {
      ActionUtility::enable(dataAwareActions_, databaseModel()->isOpen());
    }

  } // namespace chrono
} //namespace ma
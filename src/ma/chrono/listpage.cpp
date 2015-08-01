/*
TRANSLATOR ma::chrono::ListPage
*/

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/ref.hpp>
#include <boost/make_shared.hpp>
#include <QAbstractTableModel>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QFrame>
#include <QLabel>
#include <QTableView>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QPushButton>
#include <QToolButton>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/taskwindowmanager.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/actionutility.h>
#include <ma/chrono/listtaskwindow.h>
#include <ma/chrono/cardtaskwindow.h>
#include <ma/chrono/listpage.h>
#include <ma/chrono/filterdialog.h>
#include <ma/chrono/sortdialog.h>
#include <ma/chrono/excelexporter.h>

namespace ma
{
  namespace chrono
  {    
    ListPage::ListPage(const ResourceManagerPtr& resourceManager,
      const TaskWindowManagerWeakPtr& taskWindowManager,
      const DatabaseModelPtr& databaseModel,
      const QActionSharedPtr& helpAction, Entity entity, 
      const TableDescription& tableDescription, 
      const UserDefinedFilter& userDefinedFilter, 
      const UserDefinedOrder& userDefinedOrder,
      QWidget* parent)
      : DataPage(resourceManager, taskWindowManager, databaseModel, helpAction, parent)      
      , entity_(entity)      
      , primaryActions_()
      , contextActions_()
      , dataAwareActions_()
      , tableDescription_(tableDescription)
      , userDefinedFilter_(userDefinedFilter)
      , userDefinedOrder_(userDefinedOrder)
    {
      actionsMenu_ = new QMenu(this);
      rowContextMenu_ = new QMenu(this);

      viewAction_ = new QAction(resourceManager->getIcon(viewIconName), tr("&View"), this);      
      QObject::connect(viewAction_, SIGNAL(triggered()), SLOT(on_viewAction_triggered()));      
      editAction_ = new QAction(resourceManager->getIcon(editIconName), tr("&Edit"), this);
      QObject::connect(editAction_, SIGNAL(triggered()), SLOT(on_editAction_triggered()));      
      createAction_ = new QAction(resourceManager->getIcon(createNewIconName), tr("&New"), this);
      QObject::connect(createAction_, SIGNAL(triggered()), SLOT(on_createAction_triggered()));      
      deleteAction_ = new QAction(resourceManager->getIcon(deleteIconName), tr("&Delete"), this);
      QObject::connect(deleteAction_, SIGNAL(triggered()), SLOT(on_deleteAction_triggered()));      
      processActionsSeparator_ = new QAction(this);
      processActionsSeparator_->setSeparator(true);      
      specialActionsSeparator_ = new QAction(this);
      specialActionsSeparator_->setSeparator(true);      
      openInNewWindowAction_ = new QAction(tr("Open in new window"), this);
      openInNewWindowAction_->setVisible(0 != parent);
      QObject::connect(openInNewWindowAction_, SIGNAL(triggered()), SLOT(on_openInNewWindowAction_triggered()));            
      refreshAction_ = new QAction(resourceManager->getIcon(refreshIconName), tr("Re&fresh"), this);      
      QObject::connect(refreshAction_, SIGNAL(triggered()), SLOT(on_refreshAction_triggered()));      
      //clearFilterAction_ = new QAction(tr("Clear filter"), this);   
      //QObject::connect(clearFilterAction_, SIGNAL(triggered()), SLOT(on_clearFilterAction_triggered()));      
      sendToExcelAction_ = new QAction(tr("Send to Excel"), this);   
      QObject::connect(sendToExcelAction_, SIGNAL(triggered()), SLOT(on_sendToExcelAction_triggered()));      
      commonActionsSeparator_ = new QAction(this);
      commonActionsSeparator_->setSeparator(true);      
      printPageAction_ = new QAction(resourceManager->getIcon(printIconName), tr("&Print page"), this);
      QObject::connect(printPageAction_, SIGNAL(triggered()), SLOT(on_printPageAction_triggered()));      
      
      actionsMenu_->addAction(viewAction_);
      actionsMenu_->addAction(editAction_);
      actionsMenu_->addAction(createAction_);
      actionsMenu_->addAction(deleteAction_);
      actionsMenu_->addAction(processActionsSeparator_);
      actionsMenu_->addAction(specialActionsSeparator_);
      actionsMenu_->addAction(openInNewWindowAction_);
      actionsMenu_->addAction(refreshAction_);
      //actionsMenu_->addAction(clearFilterAction_);
      actionsMenu_->addAction(sendToExcelAction_);
      //actionsMenu_->addAction(commonActionsSeparator_);
      //actionsMenu_->addAction(printPageAction_);      

      rowContextMenu_->addAction(viewAction_);
      rowContextMenu_->addAction(editAction_);
      rowContextMenu_->addAction(createAction_);
      rowContextMenu_->addAction(deleteAction_);
      rowContextMenu_->addAction(processActionsSeparator_);
      rowContextMenu_->addAction(specialActionsSeparator_);      
      rowContextMenu_->addAction(refreshAction_);      
      rowContextMenu_->addAction(sendToExcelAction_);
      //rowContextMenu_->addAction(commonActionsSeparator_);
      //rowContextMenu_->addAction(printPageAction_);
    
      QAction* actionsAction = new QAction(resourceManager->getIcon(actionIconName), tr("&Actions"), this);
      actionsAction->setMenu(actionsMenu_);

      primaryActions_ << actionsAction;
      //todo: add customizeMenu
      customizeAction_ = 0;
      contextActions_ << openInNewWindowAction_;
      dataAwareActions_ << viewAction_ << editAction_ << createAction_ << deleteAction_;
      itemAwareActions_ << viewAction_ << editAction_ << deleteAction_;
      
      QVBoxLayout* layout = new QVBoxLayout(this);
      layout->setContentsMargins(0, 0, 0, 0);
      layout->setSpacing(0);
            
      filterPane_ = new QFrame(this);      
      QHBoxLayout* filterPaneLayout = new QHBoxLayout(filterPane_);           
      
      QToolButton* enableFilterBtn = new QToolButton(filterPane_);
      enableFilterBtn_ = enableFilterBtn;      
      enableFilterBtn->setText(tr("Filter"));
      //enableFilterBtn->setIcon(resourceManager->getIcon(filterIconName));      
      enableFilterBtn->setCheckable(true);
      enableFilterBtn->setChecked(userDefinedFilter.enabled);
      enableFilterBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
      enableFilterBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
      QObject::connect(enableFilterBtn, SIGNAL(clicked(bool)), SLOT(on_enableFilterBtn_clicked(bool)));      
      filterPaneLayout->addWidget(enableFilterBtn);
      customizeFilterBtn_ = new QToolButton(filterPane_);
      customizeFilterBtn_->setText(tr("..."));
      customizeFilterBtn_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
      QObject::connect(customizeFilterBtn_, SIGNAL(clicked(bool)), SLOT(on_customizeFilterBtn_clicked(bool)));      
      filterPaneLayout->addWidget(customizeFilterBtn_);     
      
      QToolButton* enableSortBtn = new QToolButton(filterPane_);
      enableSortBtn_ = enableSortBtn;      
      enableSortBtn->setText(tr("Sort"));
      enableSortBtn->setCheckable(true);    
      enableSortBtn->setChecked(userDefinedOrder.enabled);
      enableSortBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
      enableSortBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
      QObject::connect(enableSortBtn, SIGNAL(clicked(bool)), SLOT(on_enableSortBtn_clicked(bool)));      
      filterPaneLayout->addStretch();
      filterPaneLayout->addWidget(enableSortBtn);     
      customizeSortBtn_ = new QToolButton(filterPane_);
      customizeSortBtn_->setText(tr("..."));
      customizeSortBtn_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
      QObject::connect(customizeSortBtn_, SIGNAL(clicked(bool)), SLOT(on_customizeSortBtn_clicked(bool)));            
      filterPaneLayout->addWidget(customizeSortBtn_);

      layout->addWidget(filterPane_);

      filterDialog_ = new FilterDialog(resourceManager, tableDescription, taskWindowManager, databaseModel, helpAction, this);
      sortDialog_   = new SortDialog(resourceManager, tableDescription, taskWindowManager, databaseModel, helpAction, this);
      
      tableView_ = new QTableView(this);
      tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
      tableView_->setContextMenuPolicy(Qt::CustomContextMenu);
      tableView_->setWordWrap(false);
      //tableView_->setAlternatingRowColors(true);
      tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
      tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
      tableView_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
      tableView_->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);                    
      QHeaderView* header = tableView_->horizontalHeader();

      // Qt 5 migration
#if QT_VERSION >= 0x050000      
      header->setSectionsMovable(true);
#else
      header->setMovable(true);
#endif

      header->setContextMenuPolicy(Qt::CustomContextMenu);
      header->setHighlightSections(false);
      //header->setSortIndicatorShown(true);
      header->setStretchLastSection(true);
      QObject::connect(header, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(on_headerContextMenu_requested(const QPoint&)));
      QObject::connect(tableView_, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(on_tableViewContextMenu_requested(const QPoint&)));      
      QObject::connect(tableView_, SIGNAL(doubleClicked(const QModelIndex&)), SIGNAL(doubleClicked(const QModelIndex&)));      
      tableView_->verticalHeader()->setVisible(false);      
      layout->addWidget(tableView_);      

      QObject::connect(databaseModel.get(), SIGNAL(connectionStateChanged()), SLOT(on_databaseModel_connectionStateChanged()));      
      on_databaseModel_connectionStateChanged();
      updateState();
    }

    ListPage::~ListPage()
    {
    }

    QTableView* ListPage::tableView() const
    {
      return tableView_;
    }

    Entity ListPage::entity() const
    {
      return entity_;
    }    

    QAbstractItemModel* ListPage::model() const
    {
      return tableView_->model();
    }

    void ListPage::setModel(QAbstractItemModel* model)
    {
      std::auto_ptr<QItemSelectionModel> selectionModel(tableView_->selectionModel());
      tableView_->setModel(model);
      if (model)
      {      
        QHeaderView* header = tableView_->horizontalHeader();

        // Qt 5 migration
#if QT_VERSION >= 0x050000      
        header->setSectionsMovable(true);
#else
        header->setMovable(true);
#endif

        header->setSectionHidden(0, true);      
        tableView_->resizeColumnsToContents();
      }
      if (QItemSelectionModel* selectionModel = tableView_->selectionModel())
      {
        QObject::connect(selectionModel, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), 
          SLOT(on_currentChanged(const QModelIndex&, const QModelIndex&)));
      }      
    }

    QActionList ListPage::primaryActions() const
    {
      return primaryActions_;
    }

    QAction* ListPage::customizeAction() const
    {
      return customizeAction_;
    }
     
    QActionList ListPage::contextActions() const
    {
      return contextActions_;
    }        

    QAction* ListPage::viewAction() const
    {
      return viewAction_;
    }

    QAction* ListPage::editAction() const
    {
      return editAction_;
    }

    QAction* ListPage::createAction() const
    {
      return createAction_;
    }

    QAction* ListPage::deleteAction() const
    {
      return deleteAction_;
    }

    QAction* ListPage::openInNewWindowAction() const
    {
      return openInNewWindowAction_;
    }

    QAction* ListPage::refreshAction() const
    {
      return refreshAction_;
    }

    //QAction* ListPage::clearFilterAction() const
    //{
    //  return clearFilterAction_;
    //}

    QAction* ListPage::sendToExcelAction() const
    {
      return sendToExcelAction_;
    }

    QAction* ListPage::printPageAction() const
    {
      return printPageAction_;
    }

    void ListPage::addPrimaryActions(const QActionList& actions)
    {
      primaryActions_ << actions;      
    }

    void ListPage::addSpecialActions(const QActionList& actions)
    {
      actionsMenu_->insertActions(specialActionsSeparator_, actions);
      rowContextMenu_->insertActions(specialActionsSeparator_, actions);
    }

    void ListPage::viewSelected()
    {
      showSelectedCardTaskWindow(CardPage::viewMode);
    }

    void ListPage::editSelected()
    {
      boost::shared_ptr<CardTaskWindow> cardTaskWindow = showSelectedCardTaskWindow(CardPage::editMode);
      if (cardTaskWindow)
      {      
        CardPage* cardPage = cardTaskWindow->cardPage();
        cardPage->edit();
      }
    }

    void ListPage::createNew()
    {
      TaskWindowManagerPtr taskWindowManager = this->taskWindowManager().lock();
      if (taskWindowManager)
      {
        std::auto_ptr<CardPage> cardPage = 
            createCardPage(CardPage::createMode, OptionalQInt64());
        boost::shared_ptr<CardTaskWindow> cardTaskWindow = 
            boost::make_shared<CardTaskWindow>(boost::ref(cardPage));
        taskWindowManager->add(cardTaskWindow);
        cardTaskWindow->show();
      }
    }

    void ListPage::removeSelected()
    {
      boost::shared_ptr<CardTaskWindow> cardTaskWindow = showSelectedCardTaskWindow(CardPage::viewMode);
      if (cardTaskWindow)
      {      
        CardPage* cardPage = cardTaskWindow->cardPage();
        cardPage->remove();
      }
    }    

    OptionalQInt64 ListPage::selectedId() const
    {
      QModelIndex index = tableView_->currentIndex();
      if (!index.isValid())
      {
        return OptionalQInt64();
      }
      return entityIdFromRow(index.row());
    }

    OptionalQString ListPage::selectedRelationalText() const
    {
      QModelIndex index = tableView_->currentIndex();
      if (!index.isValid())
      {
        return OptionalQString();
      }
      return relationalTextFromRow(index.row());
    }    

    TableDescription ListPage::tableDescription() const
    {
      return tableDescription_;
    }

    std::auto_ptr<CardPage> ListPage::createCardPage(CardPage::Mode mode, const OptionalQInt64& entityId) const
    {
      std::auto_ptr<CardPage> cardPage(new CardPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), entity_, mode, entityId));
      return cardPage;
    }

    OptionalQInt64 ListPage::entityIdFromRow(int row) const
    {
      if (QAbstractItemModel* model = this->model())
      {
        QVariant var = model->data(model->index(row, 0));
        if (!var.isNull())
        {          
          if (var.canConvert<qint64>())
          {
            return var.toLongLong();
          }        
        }        
      }
      return OptionalQInt64();
    }

    OptionalQString ListPage::relationalTextFromRow(int row) const
    {
      if (QAbstractItemModel* model = this->model())
      {
        QVariant var = model->data(model->index(row, 1));
        if (!var.isNull())
        {          
          if (var.canConvert<QString>())
          {
            return var.toString();
          }        
        }        
      }
      return OptionalQString();
    }

    void ListPage::setUserDefinedFilter(const UserDefinedFilter& userDefinedFilter)
    {
      userDefinedFilter_ = userDefinedFilter;
      applyUserDefinedFilter(userDefinedFilter);
    }    

    UserDefinedFilter ListPage::userDefinedFilter() const
    {
      return userDefinedFilter_;
    }

    void ListPage::setUserDefinedOrder(const UserDefinedOrder& userDefinedOrder)
    {
      userDefinedOrder_ = userDefinedOrder;
      applyUserDefinedOrder(userDefinedOrder);
    }    

    UserDefinedOrder ListPage::userDefinedOrder() const
    {
      return userDefinedOrder_;
    }

    void ListPage::updateState()
    {      
      enableFilterBtn_->setEnabled(!userDefinedFilter_.filterExpression.isEmpty());
      enableSortBtn_->setEnabled(!userDefinedOrder_.orderExpression.isEmpty());
      updateActions();
    }
  
    void ListPage::updateActions()
    {
      ActionUtility::enable(dataAwareActions_, databaseModel()->isOpen());
      if (!tableView_->currentIndex().isValid())
      {
        ActionUtility::enable(itemAwareActions_, false);
      }      
    }

    boost::shared_ptr<CardTaskWindow> ListPage::showSelectedCardTaskWindow(CardPage::Mode mode) const
    {
      boost::shared_ptr<CardTaskWindow> cardTaskWindow;
      if (TaskWindowManagerPtr taskWindowManager = this->taskWindowManager().lock())
      {              
        OptionalQInt64 entityId = selectedId();
        if (entityId)
        {        
          cardTaskWindow = taskWindowManager->findCardTaskWindow(entity_, entityId.get());
          if (cardTaskWindow)
          {
            if (cardTaskWindow->isMinimized())
            {
              cardTaskWindow->setWindowState(cardTaskWindow->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
            }
            cardTaskWindow->show();
            cardTaskWindow->activateWindow();
            cardTaskWindow->raise();        
          }
          else
          {
            std::auto_ptr<CardPage> cardPage(createCardPage(mode, entityId));
            cardTaskWindow = boost::make_shared<CardTaskWindow>(boost::ref(cardPage));
            taskWindowManager->add(cardTaskWindow);
            cardTaskWindow->show();
          }
        }
      }
      return cardTaskWindow;
    }    

    void ListPage::on_viewAction_triggered()
    {
      viewSelected();
    }

    void ListPage::on_editAction_triggered()
    {
      editSelected();
    }

    void ListPage::on_createAction_triggered()
    {    
      createNew();
    }

    void ListPage::on_deleteAction_triggered()
    {
      removeSelected();
    }

    void ListPage::on_openInNewWindowAction_triggered()
    {      
      TaskWindowManagerPtr taskWindowManager(this->taskWindowManager().lock());
      if (taskWindowManager)
      {
        std::auto_ptr<ListPage> clonedPage = this->clone();
        boost::shared_ptr<ListTaskWindow> listTaskWindow = 
            boost::make_shared<ListTaskWindow>(boost::ref(clonedPage));
        taskWindowManager->add(listTaskWindow);      
        listTaskWindow->show();
      }            
    }

    void ListPage::on_refreshAction_triggered()
    {
      refresh();
    }

    //void ListPage::on_clearFilterAction_triggered()
    //{
    //  //todo
    //}

    void ListPage::on_sendToExcelAction_triggered()
    {      
      if (!ExcelExporter::exportTable(*tableView_))
      {
        //todo
      }      
    }

    void ListPage::on_printPageAction_triggered()
    {
      //todo
    }

    void ListPage::on_headerContextMenu_requested(const QPoint& pos)
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
            if (int section = header->logicalIndex(i))
            {
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
          }        
          if (!contextMenu.isEmpty())
          {
            QAction* action = contextMenu.exec(header->mapToGlobal(pos));
            int index = contextMenu.actions().indexOf(action);            
            if (0 <= index)
            {
              if (index >= header->visualIndex(0))
              {
                ++index;
              }
              int section = header->logicalIndex(index);              
              header->setSectionHidden(section, !header->isSectionHidden(section));
            }
          }
        }
      }
    }

    void ListPage::on_tableViewContextMenu_requested(const QPoint& pos)
    {
      if (QAbstractItemModel* model = tableView_->model())
      {
        QModelIndex index(tableView_->indexAt(pos));
        if (index.isValid())
        {
          rowContextMenu_->exec(tableView_->viewport()->mapToGlobal(pos));
        }
      }                 
    }

    void ListPage::on_databaseModel_connectionStateChanged()
    {
      updateActions();
    }

    void ListPage::on_currentChanged(const QModelIndex& current, const QModelIndex& previous)
    {
      updateActions();
      emit currentChanged(current, previous);
    }

    void ListPage::on_enableFilterBtn_clicked(bool checked)
    {
      userDefinedFilter_.enabled = checked;
      setUserDefinedFilter(userDefinedFilter_);
      updateState();
    }

    void ListPage::on_customizeFilterBtn_clicked(bool /*checked*/)
    {
      filterDialog_->setFilterExpression(userDefinedFilter_.filterExpression);
      if (QDialog::Accepted == filterDialog_->exec())
      {
        userDefinedFilter_.filterExpression = filterDialog_->filterExpression();
        if (userDefinedFilter_.filterExpression.isEmpty())
        {
          userDefinedFilter_.enabled = false;
          enableFilterBtn_->setChecked(false);          
        }
        else
        {
          userDefinedFilter_.enabled = true;
          enableFilterBtn_->setChecked(true);
        }
        setUserDefinedFilter(userDefinedFilter_);
        updateState();
      }                        
    }

    void ListPage::on_enableSortBtn_clicked(bool checked)
    {
      userDefinedOrder_.enabled = checked;
      setUserDefinedOrder(userDefinedOrder_);
      updateState();
    }
      
    void ListPage::on_customizeSortBtn_clicked(bool /*checked*/)
    {
      sortDialog_->setOrderExpression(userDefinedOrder_.orderExpression);
      if (QDialog::Accepted == sortDialog_->exec())
      {        
        userDefinedOrder_.orderExpression = sortDialog_->orderExpression();
        if (userDefinedOrder_.orderExpression.isEmpty())
        {
          userDefinedOrder_.enabled = false;
          enableSortBtn_->setChecked(false);
        }
        else
        {
          userDefinedOrder_.enabled = true;
          enableSortBtn_->setChecked(true);
        }
        setUserDefinedOrder(userDefinedOrder_);
        updateState();
      }
    }    

  } // namespace chrono
} // namespace ma
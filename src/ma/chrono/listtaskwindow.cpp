/*
TRANSLATOR ma::chrono::ListTaskWindow
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <QIcon>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QTableView>
#include <ma/chrono/commandbar.h>
#include <ma/chrono/listpage.h>
#include <ma/chrono/listtaskwindow.h>

namespace ma
{
  namespace chrono
  {
    ListTaskWindow::ListTaskWindow(std::auto_ptr<ListPage> listPage)
      : TaskWindow()      
    {
      setWindowIcon(listPage->windowIcon());
      setWindowTitle(listPage->windowTitle());

      QWidget* contents = this->contents();
      QVBoxLayout* contentsLayout = new QVBoxLayout(contents);
      contentsLayout->setContentsMargins(0, 0, 0, 0);
      contentsLayout->setSpacing(0);

      CommandBar* commandBar = this->commandBar();
      commandBar->addCommands(listPage->primaryActions());
      commandBar->addCommand(listPage->helpAction().get(), CommandBar::tray, false);      
      if (QAction* customizeAction = listPage->customizeAction())
      {
        commandBar->addCommand(customizeAction, CommandBar::tray, false);
      }

      listPage->setParent(contents);      
      contentsLayout->addWidget(listPage.get());
      listPage_ = listPage.release();            
      
      QWidget* footer = new QWidget(contents);
      footer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
      contentsLayout->addWidget(footer);
      QVBoxLayout* footerLayout = new QVBoxLayout(footer);
      QDialogButtonBox* buttonBox = new QDialogButtonBox(footer);
      footerLayout->addWidget(buttonBox);
      buttonBox->setStandardButtons(QDialogButtonBox::Close);
      //buttonBox->button(QDialogButtonBox::Close)->setDefault(false);

      QObject::connect(listPage_, SIGNAL(doubleClicked(const QModelIndex&)), SLOT(on_listPage_doubleClicked(const QModelIndex&)));
      QObject::connect(listPage_, SIGNAL(windowTitleUpdated()), SLOT(on_listPage_windowTitleUpdated()));
      QObject::connect(listPage_, SIGNAL(needToClose()), SLOT(close()));
      QObject::connect(buttonBox, SIGNAL(rejected()), SLOT(close()));
    }

    ListTaskWindow::~ListTaskWindow()
    {
    }

    ListPage* ListTaskWindow::listPage() const
    {
      return listPage_;
    }    

    void ListTaskWindow::on_listPage_windowTitleUpdated()
    {    
      setWindowTitle(listPage_->windowTitle());
    }

    void ListTaskWindow::on_listPage_doubleClicked(const QModelIndex& /*index*/)
    {
      listPage_->viewSelected();
    }

  } // namespace chrono
} //namespace ma
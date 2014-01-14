/*
TRANSLATOR ma::chrono::CardTaskWindow
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <QIcon>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <ma/chrono/commandbar.h>
#include <ma/chrono/cardpage.h>
#include <ma/chrono/cardtaskwindow.h>

namespace ma
{
  namespace chrono
  {
    CardTaskWindow::CardTaskWindow(std::auto_ptr<CardPage> cardPage)
      : TaskWindow()      
    {
      setWindowIcon(cardPage->windowIcon());
      setWindowTitle(cardPage->windowTitle());

      QSize cardPageSize = cardPage->size();
      QWidget* contents = this->contents();
      QVBoxLayout* contentsLayout = new QVBoxLayout(contents);      
      
      CommandBar* commandBar = this->commandBar();
      commandBar->addCommands(cardPage->primaryActions());
      commandBar->addCommand(cardPage->helpAction().get(), CommandBar::tray, false);      
      if (QAction* customizeAction = cardPage->customizeAction())
      {
        commandBar->addCommand(customizeAction, CommandBar::tray, false);
      }
      
      cardPage->setParent(contents);      
      contentsLayout->addWidget(cardPage.get());
      cardPage_ = cardPage.release();
            
      buttonBox_ = new QDialogButtonBox(contents);
      QObject::connect(buttonBox_, SIGNAL(accepted()), SLOT(on_buttonBox_accepted()));
      QObject::connect(buttonBox_, SIGNAL(rejected()), SLOT(on_buttonBox_rejected()));
      QObject::connect(this, SIGNAL(rejected()), buttonBox_, SIGNAL(rejected()));      
      contentsLayout->addWidget(buttonBox_);
      
      resize(cardPageSize.width(), commandBar->height() + cardPageSize.height() + buttonBox_->height() + 9 * 2);

      QObject::connect(cardPage_, SIGNAL(modeChanged()), SLOT(on_cardPage_modeChanged()));
      QObject::connect(cardPage_, SIGNAL(windowTitleUpdated()), SLOT(on_cardPage_windowTitleUpdated()));
      QObject::connect(cardPage_, SIGNAL(needToClose()), SLOT(close()));            
      on_cardPage_modeChanged();      
    }

    CardTaskWindow::~CardTaskWindow()
    {
    }

    CardPage* CardTaskWindow::cardPage() const
    {
      return cardPage_;
    }

    void CardTaskWindow::on_cardPage_modeChanged()
    {
      switch (cardPage_->mode())
      {
      case CardPage::viewMode:
        buttonBox_->setStandardButtons(QDialogButtonBox::Close);
        break;
      case CardPage::editMode:
      case CardPage::createMode:
        buttonBox_->setStandardButtons(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
        break;
      default:
        buttonBox_->setStandardButtons(QDialogButtonBox::NoButton);
        break;
      }
    }

    void CardTaskWindow::on_cardPage_windowTitleUpdated()
    {      
      setWindowTitle(cardPage_->windowTitle());
    }

    void CardTaskWindow::on_buttonBox_accepted()
    {
      switch (cardPage_->mode())
      {
      case CardPage::viewMode:
        close();
        break;
      case CardPage::editMode:
      case CardPage::createMode:
        cardPage_->save();
        break;      
      } 
    }

    void CardTaskWindow::on_buttonBox_rejected()
    {
      switch (cardPage_->mode())
      {
      case CardPage::viewMode:
        close();
        break;
      case CardPage::editMode:
      case CardPage::createMode:
        cardPage_->cancel();
        break;
      }
    }    

  } // namespace chrono
} // namespace ma
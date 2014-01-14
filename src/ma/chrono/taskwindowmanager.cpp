//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ma/chrono/listtaskwindow.h>
#include <ma/chrono/cardpage.h>
#include <ma/chrono/cardtaskwindow.h>
#include <ma/chrono/taskwindowmanager.h>

namespace ma
{
  namespace chrono
  {    
    TaskWindowManager::TaskWindowManager()
      : QObject(0)
      , listTaskWindows_()
      , cardTaskWindows_()      
    {
    }

    TaskWindowManager::~TaskWindowManager()
    {
    }    

    void TaskWindowManager::add(const boost::shared_ptr<ListTaskWindow>& listTaskWindow)
    {
      QObject::connect(listTaskWindow.get(), SIGNAL(finished(int)), SLOT(removeListTaskWindow(int)), Qt::QueuedConnection);
      listTaskWindows_.insert(listTaskWindow.get(), listTaskWindow);
    }    

    void TaskWindowManager::add(const boost::shared_ptr<CardTaskWindow>& cardTaskWindow)
    {
      QObject::connect(cardTaskWindow.get(), SIGNAL(finished(int)), SLOT(removeCardTaskWindow(int)), Qt::QueuedConnection);
      cardTaskWindows_.insert(cardTaskWindow.get(), cardTaskWindow);
    }

    boost::shared_ptr<CardTaskWindow> TaskWindowManager::findCardTaskWindow(Entity entity, qint64 entityId)
    {            
      typedef QMap<CardTaskWindow*, boost::shared_ptr<CardTaskWindow> > map;
      typedef map::const_iterator const_iterator;
      
      for (const_iterator it = cardTaskWindows_.begin(), end = cardTaskWindows_.end(); it != end; ++it)
      {
        CardPage* cardPage = it.key()->cardPage();
        OptionalQInt64 cardPageEntityId = cardPage->entityId();
        if (cardPage->entity() == entity && cardPageEntityId)
        {
          if (cardPageEntityId.get() == entityId)
          {
            return it.value(); 
          }
        }        
      }
      return boost::shared_ptr<CardTaskWindow>();
    }

    void TaskWindowManager::removeListTaskWindow(int)
    { 
      ListTaskWindow* listTaskWindow = qobject_cast<ListTaskWindow*>(sender());
      listTaskWindows_.remove(listTaskWindow);      
    }

    void TaskWindowManager::removeCardTaskWindow(int)
    { 
      CardTaskWindow* cardTaskWindow = qobject_cast<CardTaskWindow*>(sender());
      cardTaskWindows_.remove(cardTaskWindow);
    }

  } // namespace chrono
} // namespace ma

/*
TRANSLATOR ma::chrono::Page 
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <ma/chrono/page.h>

namespace ma {

namespace chrono {

Page::Page(const ResourceManagerPtr& resourceManager,
  const TaskWindowManagerWeakPtr& taskWindowManager,
  const QActionSharedPtr& helpAction,
  QWidget* parent)
  : QWidget(parent)      
  , resourceManager_(resourceManager)
  , taskWindowManager_(taskWindowManager)
  , helpAction_(helpAction)
{      
}

Page::~Page()
{
}      

ResourceManagerPtr Page::resourceManager() const
{
  return resourceManager_;
}    

TaskWindowManagerWeakPtr Page::taskWindowManager() const
{
  return taskWindowManager_;
} 

QActionSharedPtr Page::helpAction() const
{
  return helpAction_;
}
    
} // namespace chrono
} //namespace ma

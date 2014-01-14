/*
TRANSLATOR ma::chrono::Page 
*/

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
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
} // namespace ma

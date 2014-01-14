//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <QAction>
#include <ma/chrono/actionutility.h>

namespace ma {
namespace chrono {

void ActionUtility::enable(const QActionList& actions, bool enabled)
{
  typedef QActionList action_list;
  typedef action_list::const_iterator action_iterator;

  for (action_iterator it = actions.begin(), end = actions.end();
      it != end; ++it)
  {
    (*it)->setEnabled(enabled);
  }
}    

void ActionUtility::show(const QActionList& actions, bool visible)
{
  typedef QActionList action_list;
  typedef action_list::const_iterator action_iterator;

  for (action_iterator it = actions.begin(), end = actions.end();
      it != end; ++it)
  {
    (*it)->setVisible(visible);
  }
}

} // namespace chrono
} // namespace ma

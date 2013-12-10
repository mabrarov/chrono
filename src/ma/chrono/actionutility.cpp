//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
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
} //namespace ma

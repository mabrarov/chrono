//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_COMMANDBAR_H
#define MA_CHRONO_COMMANDBAR_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/config.hpp>
#include <QList>
#include <QWidget>
#include <ma/chrono/types.h>
#include <ma/chrono/commandbutton_fwd.h>
#include <ma/chrono/commandbar_fwd.h>

QT_BEGIN_NAMESPACE
class QAction;
class QLayout;
class QBoxLayout;  
QT_END_NAMESPACE

namespace ma {
namespace chrono {

class CommandBar : public QWidget
{
  Q_OBJECT

public:
  enum CommandArea {main, tray};      

  explicit CommandBar(QWidget* parent);
  ~CommandBar();
  QSize minimumSizeHint() const;

  void addCommand(QAction* action, CommandArea area = main, 
      bool showText = true);
  void addCommands(const QActionList& actions, CommandArea area = main, 
      bool showText = true);
  void removeCommands(QAction* action);
  void removeCommands(QAction* action, CommandArea area);
  void removeCommands(const QActionList& actions);
  bool hasCommand(QAction* action) const;
  bool hasCommand(QAction* action, CommandArea area) const;

protected:
  void paintEvent(QPaintEvent* event);

private:
  BOOST_STATIC_CONSTANT(int, minimumSizeHeight = 24);
  BOOST_STATIC_CONSTANT(int, borderWidth  = 1);
  BOOST_STATIC_CONSTANT(int, borderHeight = 1);

  Q_DISABLE_COPY(CommandBar)

  void updateTabOrder();
  void addCommand(QBoxLayout* layout, QAction* action, bool showText);
  static QWidget* updateTabOrder(QLayout* layout, QWidget* first = 0);
  static CommandButton* hasCommand(QLayout* layout, QAction* action);
  static void removeCommands(QLayout* layout, QAction* action);
  static QWidget* findFocusProxy(QLayout* layout);      

  QBoxLayout* mainLayout_;      
  QBoxLayout* trayLayout_;      
}; // class CommandBar

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_COMMANDBAR_H

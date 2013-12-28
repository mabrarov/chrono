//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_CARDTASKWINDOW_H
#define MA_CHRONO_CARDTASKWINDOW_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <memory>
#include <ma/chrono/cardpage_fwd.h>
#include <ma/chrono/taskwindow.h>
#include <ma/chrono/cardtaskwindow_fwd.h>

QT_BEGIN_NAMESPACE  
class QDialogButtonBox;  
QT_END_NAMESPACE

namespace ma {
namespace chrono {

class CardTaskWindow : public TaskWindow
{
  Q_OBJECT

public:
  explicit CardTaskWindow(std::auto_ptr<CardPage> cardPage);
  ~CardTaskWindow();
  CardPage* cardPage() const;

private:
  Q_DISABLE_COPY(CardTaskWindow)
      
  CardPage* cardPage_;
  QDialogButtonBox* buttonBox_;

private slots:
  void on_cardPage_modeChanged();
  void on_cardPage_windowTitleUpdated();
  void on_buttonBox_accepted();
  void on_buttonBox_rejected();                  
}; // class CardTaskWindow

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_CARDTASKWINDOW_H

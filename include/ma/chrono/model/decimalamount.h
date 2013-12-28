//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_MODEL_DECIMALAMOUNT_H
#define MA_CHRONO_MODEL_DECIMALAMOUNT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <QtGlobal>
#include <QMetaType>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

namespace ma {
namespace chrono {
namespace model {

class DecimalAmount
{
public:        
  DecimalAmount(int fracDiv, int fracDigits);
  DecimalAmount(int fracDiv, int fracDigits, qint64 amount);
      
  qint64 amount() const;
  qint64 intPart() const;
  qint64 fracPart() const;
  QString toString() const;
  double toDouble() const;
  bool setValue(const QString& text);

private:
  int    fracDiv_;
  int    fracDigits_;
  qint64 amount_;         
}; // class DecimalAmount      

} // namespace model    
} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_MODEL_DECIMALAMOUNT_H

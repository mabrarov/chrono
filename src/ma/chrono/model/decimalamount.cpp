//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <algorithm>
#include <QString>
#include <QLocale>
#include <ma/chrono/model/decimalamount.h>

namespace ma {
namespace chrono {        
namespace model {

DecimalAmount::DecimalAmount(int fracDiv, int fracDigits)
  : fracDiv_(fracDiv)
  , fracDigits_(fracDigits)
  , amount_(0)          
{
} // DecimalAmount::DecimalAmount

DecimalAmount::DecimalAmount(int fracDiv, int fracDigits, qint64 amount)
  : fracDiv_(fracDiv)
  , fracDigits_(fracDigits)
  , amount_(amount)          
{
} // DecimalAmount::DecimalAmount

qint64 DecimalAmount::amount() const
{
  return amount_;
} // DecimalAmount::amount
       
qint64 DecimalAmount::intPart() const
{          
  if (0 > amount_)
  {            
    return -((-amount_) / fracDiv_);
  }
  return amount_ / fracDiv_;
} // DecimalAmount::intPart

qint64 DecimalAmount::fracPart() const
{
  if (0 > amount_)
  {            
    return -((-amount_) % fracDiv_);
  }
  return amount_ % fracDiv_;          
} // DecimalAmount::fracPart

QString DecimalAmount::toString() const
{
  return QString("%1%2%3")
      .arg(intPart())
      .arg(QLocale().decimalPoint())
      .arg(fracPart(), fracDigits_, 10, QLatin1Char('0'));
} // DecimalAmount::toString

double DecimalAmount::toDouble() const
{        
  return intPart() + static_cast<double>(fracPart()) / fracDiv_;
}

bool DecimalAmount::setValue(const QString& text)
{
  const QString delimiters(QString::fromWCharArray(L".,-=/><сўСо?"));
  int decimalPointIndex = -1;
  for (QString::const_iterator i = delimiters.begin(), e = delimiters.end();
      i != e; ++i)
  {
    decimalPointIndex = text.indexOf(*i);
    if (-1 != decimalPointIndex)
    {
      break;
    }
  }

  QString intPartText;
  QString fracPartText;
  if (-1 == decimalPointIndex)
  {
    intPartText = text;
  }
  else
  {
    intPartText = text.left(decimalPointIndex);
    fracPartText = text.right(text.length() - decimalPointIndex - 1);            
  }

  int fracPartTextLen = fracPartText.length();
  if (fracPartTextLen > fracDigits_)
  {
    return false;
  }
  else
  {
    for (int i = fracPartTextLen; i != fracDigits_; ++i)
    {
      fracPartText.append('0');
    }
  }

  bool ok = false;
  qint64 intPart = intPartText.toLongLong(&ok);
  if (!ok)
  {
    return false;
  }
  int fracPart = fracPartText.toInt(&ok);
  if (!ok)
  {
    return false;
  }
  if (0 > fracPart)
  {
    return false;
  }

  intPart *= fracDiv_;
  if (0 > intPart)
  {
    amount_ = intPart - fracPart;
  }
  else
  {
    amount_ = intPart + fracPart;
  }          
  return true;
} // DecimalAmount::setValue

} // namespace model
} // namespace chrono
} //namespace ma

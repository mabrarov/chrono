//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_TYPES_H
#define MA_CHRONO_TYPES_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <qglobal.h>
#include <QList>
#include <QString>
#include <QDate>
#include <QDateTime>
#include <QVariant>
#include <ma/chrono/types_fwd.h>

QT_BEGIN_NAMESPACE
class QAction;
class QWidget;
QT_END_NAMESPACE

namespace ma {
namespace chrono {

typedef boost::optional<QString>   OptionalQString;
typedef boost::optional<QDate>     OptionalQDate;
typedef boost::optional<QDateTime> OptionalQDateTime;
typedef boost::optional<int>       OptionalInt;
typedef boost::optional<qint32>    OptionalQInt32;
typedef boost::optional<qint64>    OptionalQInt64;
typedef boost::shared_ptr<QAction> QActionSharedPtr;

// QString -> OptionalQString
inline OptionalQString buildOptionalQString(const QString& str)
{
  if (str.isEmpty())
  {
    return OptionalQString();
  }
  return str;
}

// OptionalQString -> QString
inline QString buildQString(const OptionalQString& str)
{
  if (str)
  {
    return str.get();
  }
  return QString();
}

// QVariant -> OptionalQString
inline OptionalQString buildOptionalQString(const QVariant& var)
{
  if (var.isNull())
  {
    return OptionalQString();
  }
  if (var.canConvert<QString>())
  {
    return var.toString();        
  }
  return OptionalQString();
}

// OptionalQString -> QVariant
inline QVariant buildQVariant(const OptionalQString& str)
{
  if (str)
  {
    return QVariant(str.get());
  }  
  return QVariant(QVariant::String);
}

typedef QList<QWidget*> QWidgetList;
typedef QList<QAction*> QActionList;

} // namespace chrono
} // namespace ma

Q_DECLARE_METATYPE(ma::chrono::OptionalQString)
Q_DECLARE_METATYPE(ma::chrono::OptionalQDate)
Q_DECLARE_METATYPE(ma::chrono::OptionalQDateTime)
Q_DECLARE_METATYPE(ma::chrono::OptionalInt)
//todo : check if OptionalInt is the same type as OptionalQInt32
// Q_DECLARE_METATYPE(ma::chrono::OptionalQInt32)
//todo : check if OptionalInt is the same type as OptionalQInt64
Q_DECLARE_METATYPE(ma::chrono::OptionalQInt64)

#endif // MA_CHRONO_TYPES_H

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_MODELUTILITY_H
#define MA_CHRONO_MODELUTILITY_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/utility.hpp>
#include <QString>
#include <QModelIndex>

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

namespace ma {
namespace chrono {    

class ModelUtility : private boost::noncopyable
{
public:
  static QString indexToPath(const QModelIndex& index, 
      const QModelIndex& rootIndex = QModelIndex());

private:
  ModelUtility(); 
}; // class ModelUtility

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_MODELUTILITY_H

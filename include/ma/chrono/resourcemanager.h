//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_RESOURCEMANAGER_H
#define MA_CHRONO_RESOURCEMANAGER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/utility.hpp>
#include <QString>
//#include <QSize>
//#include <QList>
#include <QMap>
#include <QIcon>
#include <ma/chrono/resourcemanager_fwd.h>

namespace ma {
namespace chrono {

class ResourceManager : private boost::noncopyable
{
public:
  ResourceManager();
  ~ResourceManager();
  //void addIcon(const QString& fileName, 
  //    const QList<QSize>& preloadSizes = QList<QSize>());
  QIcon getIcon(const QString& fileName);

private:      
  QMap<QString, QIcon> cachedIcons_;
}; // class ResourceManager

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_RESOURCEMANAGER_H

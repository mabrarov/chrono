//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <ma/chrono/resourcemanager.h>

namespace ma
{
  namespace chrono
  {
    ResourceManager::ResourceManager()
      : cachedIcons_()
    {
    }

    ResourceManager::~ResourceManager()
    {
    }

    //void ResourceManager::addIcon(const QString& fileName, const QList<QSize>& preloadSizes)
    //{
    //  typedef QList<QSize> size_list;
    //  typedef size_list::const_iterator size_iterator;

    //  QIcon icon(fileName);
    //  for (size_iterator it = preloadSizes.begin(), end = preloadSizes.end(); it != end; ++it)
    //  {
    //    for (int iconMode = QIcon::Normal; iconMode != QIcon::Selected; ++iconMode)
    //    { 
    //      QPixmap offPixmap(icon.pixmap(*it, static_cast<QIcon::Mode>(iconMode), QIcon::Off));
    //      (void) offPixmap;
    //      QPixmap onPixmap(icon.pixmap(*it, static_cast<QIcon::Mode>(iconMode), QIcon::On));
    //      (void) onPixmap;
    //      
    //    }
    //  }
    //  cachedIcons_.insert(fileName, icon);
    //}

    QIcon ResourceManager::getIcon(const QString& fileName)
    {
      QIcon icon(cachedIcons_.value(fileName));
      if (icon.isNull())
      {
        icon = QIcon(fileName);
        cachedIcons_.insert(fileName, icon);
      }
      return icon;      
    }
    
  } // namespace chrono
} //namespace ma
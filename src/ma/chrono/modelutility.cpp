//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <Qt>
#include <QAbstractItemModel>
#include <ma/chrono/modelutility.h>

namespace ma
{
  namespace chrono
  {        
    QString ModelUtility::indexToPath(const QModelIndex& index, const QModelIndex& rootIndex)
    {
      QString path;
      if (index.isValid())
      {
        const QAbstractItemModel* model = index.model();
        QModelIndex currentIndex(index);
        while (currentIndex.isValid() && currentIndex != rootIndex)
        {
          QString itemText = model->data(currentIndex, Qt::DisplayRole).value<QString>();
          if (!itemText.isEmpty())
          {
            if (path.isEmpty())
            {
              path = itemText;
            }
            else
            {
              path = itemText + '\\' + path;
            }            
          }          
          currentIndex = currentIndex.parent();
        }
      }
      return path;
    }    

  } // namespace chrono
} //namespace ma
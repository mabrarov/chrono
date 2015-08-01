//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <QLatin1Char>
#include <QAbstractItemModel>
#include <ma/chrono/modelutility.h>

namespace ma {
namespace chrono {

QString ModelUtility::indexToPath(
    const QModelIndex& index, const QModelIndex& rootIndex)
{
  QString path;
  if (index.isValid())
  {
    const QAbstractItemModel* model = index.model();
    QModelIndex currentIndex(index);
    while (currentIndex.isValid() && currentIndex != rootIndex)
    {
      QString itemText = 
          model->data(currentIndex, Qt::DisplayRole).value<QString>();
      if (!itemText.isEmpty())
      {
        if (path.isEmpty())
        {
          path = itemText;
        }
        else
        {
          path = itemText + QLatin1Char('\\') + path;
        }            
      }          
      currentIndex = currentIndex.parent();
    }
  }
  return path;
}    

} // namespace chrono
} // namespace ma

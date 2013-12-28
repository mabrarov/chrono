//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_BREADCRUMBBAR_H
#define MA_CHRONO_BREADCRUMBBAR_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/shared_ptr.hpp>
#include <QList>
#include <QModelIndex>
#include <QLineEdit>
#include <QIcon>
#include <ma/chrono/breadcrumbbar_fwd.h>

QT_BEGIN_NAMESPACE  
class QAbstractItemModel;  
QT_END_NAMESPACE

namespace ma {
namespace chrono {    

class BreadCrumbBar : public QLineEdit
{
  Q_OBJECT

public:
  explicit BreadCrumbBar(QWidget* parent);
  ~BreadCrumbBar();       

  void setModel(QAbstractItemModel* model);
  QAbstractItemModel* model() const;
  void setRootIndex(const QModelIndex& index);
  QModelIndex rootIndex() const;      
  QModelIndex currentIndex() const;

public slots:
  void setCurrentIndex(const QModelIndex& index);

signals:
  void selected(const QModelIndex& index);      
      
protected:      
  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void paintEvent(QPaintEvent* event);      
            
private:
  class Item;
  class ElideIndicatorItem;
  class LabelItem;
  typedef boost::shared_ptr<Item> ItemPtr;

  Q_DISABLE_COPY(BreadCrumbBar)
      
  QAbstractItemModel* model_;
  QModelIndex         rootIndex_;
  QModelIndex         currentIndex_;
  QList<ItemPtr>      items_;      
}; //class BreadCrumbBar
    
} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_BREADCRUMBBAR_H

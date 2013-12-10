//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_FILTEREXPRESSION_H
#define MA_CHRONO_FILTEREXPRESSION_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/shared_ptr.hpp>
#include <QList>
#include <QVector>
#include <ma/chrono/basicfiltercondition.h>

namespace ma {
namespace chrono {

class FilterExpression
{
public:          
  typedef boost::shared_ptr<BasicFilterCondition> ItemPtr;
  typedef QList<ItemPtr> ItemList;      

  explicit FilterExpression()
    : items_()
  {
  }

  explicit FilterExpression(const ItemList& items)
    : items_(items)
  {        
  }

  ItemList items() const
  {
    return items_;
  }

  bool isEmpty() const
  {
    return items_.isEmpty();
  }

  void addItem(const ItemPtr& item)
  {
    items_.append(item);        
  }

  void removeItem(int index)
  {
    items_.removeAt(index);
  }  

  void clear()
  {
    items_.clear();
  }

  QString sql() const
  {
    return buildSql(items_);
  }

private:
  static QString buildSql(const ItemList& items)
  {
    QString result;        
    if (!items.isEmpty())
    {
      typedef ItemList::const_iterator iterator;          
      static const QString implicationSql(" and ");                    
      for (iterator i = items.begin(), end = items.end(); i != end; ++i)
      {            
        QString sql = (*i)->sql();
        if (!result.isEmpty())
        {
          result += implicationSql;
        }
        result += QString("(") + sql + QString(")");          
      }                 
    }  
    return result;
  }

  ItemList items_;      
}; // class FilterExpression

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_FILTEREXPRESSION_H

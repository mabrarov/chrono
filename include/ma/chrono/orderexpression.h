//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_ORDEREXPRESSION_H
#define MA_CHRONO_ORDEREXPRESSION_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/shared_ptr.hpp>
#include <QList>
#include <QString>
#include <ma/chrono/fielddescription.h>

namespace ma {
namespace chrono {

class OrderExpression
{
public:
  class Item;

  typedef boost::shared_ptr<Item> ItemPtr;
  typedef QList<ItemPtr>          ItemList;

  class Item
  {
  public:
    enum OrderDirection
    {
      ascendingOrder,
      descendingOrder,
      orderDirectionCount
    }; // enum OrderDirection

    Item(const FieldDescriptionPtr& field, OrderDirection orderDirection)
      : field_(field)
      , orderDirection_(orderDirection)          
    {
      static const wchar_t* orderDirectionText[orderDirectionCount] = 
      { 
        L"\x25B2", 
        L"\x25BC"
      };
      orderText_ = QString::fromWCharArray(orderDirectionText[orderDirection])
          + field->title();

      static const char* orderDirectionSql[orderDirectionCount] = 
      {
        " asc", 
        " desc"
      };
      sql_ = field->fieldName() + QString(orderDirectionSql[orderDirection]);
    }

    FieldDescriptionPtr field() const
    {
      return field_;
    }

    OrderDirection orderDirection() const
    {
      return orderDirection_;
    }

    QString orderText() const
    {
      return orderText_;
    }

    QString sql() const
    {
      return sql_;
    }

  private:                      
    FieldDescriptionPtr field_;
    OrderDirection orderDirection_;
    QString orderText_;
    QString sql_;
  }; // class Item

  OrderExpression()
    : items_()
  {
  }

  explicit OrderExpression(const ItemList& items)
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

  bool containsField(const FieldDescriptionPtr& field) const
  {
    typedef ItemList::const_iterator iterator;
    for (iterator i = items_.begin(), end = items_.end(); i != end; ++i)
    {
      if (field == (*i)->field())
      {
        return true;
      }
    }
    return false;
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
      static const QString commaSql(", ");                    
      for (iterator i = items.begin(), end = items.end(); i != end; ++i)
      {            
        QString sql = (*i)->sql();
        if (!result.isEmpty())
        {
          result += commaSql;
        }
        result += sql;          
      }                 
    }  
    return result;
  }

  ItemList items_;
}; // class OrderExpression

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_ORDEREXPRESSION_H

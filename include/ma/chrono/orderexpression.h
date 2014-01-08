//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
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

    Item(const FieldDescriptionPtr&, OrderDirection);

    FieldDescriptionPtr field() const;    
    OrderDirection orderDirection() const;
    QString orderText() const;
    QString sql() const;

  private:                      
    FieldDescriptionPtr field_;
    OrderDirection orderDirection_;
    QString orderText_;
    QString sql_;
  }; // class Item

  OrderExpression();
  explicit OrderExpression(const ItemList&);

  ItemList items() const;
  bool isEmpty() const;
  void addItem(const ItemPtr& item);
  void removeItem(int index);
  void clear();
  bool containsField(const FieldDescriptionPtr&) const;
  QString sql() const;

private:
  static QString buildSql(const ItemList&);

  ItemList items_;
}; // class OrderExpression

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_ORDEREXPRESSION_H

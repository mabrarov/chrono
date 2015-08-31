//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ma/chrono/orderexpression.h>

namespace ma {
namespace chrono {

OrderExpression::Item::Item(
    const FieldDescriptionPtr& field, OrderDirection orderDirection)
  : field_(field)
  , orderDirection_(orderDirection)          
{
  static const wchar_t* orderDirectionText[orderDirectionCount] = 
  { 
    L"\x25B2 ", 
    L"\x25BC "
  };
  orderText_ = QString::fromWCharArray(orderDirectionText[orderDirection])
      + field->title();

  static const char* orderDirectionSql[orderDirectionCount] = 
  {
    " asc", 
    " desc"
  };
  sql_ = field->fieldName()
      + QString::fromLatin1(orderDirectionSql[orderDirection]);
}

FieldDescriptionPtr OrderExpression::Item::field() const
{
  return field_;
}

OrderExpression::Item::OrderDirection 
OrderExpression::Item::orderDirection() const
{
  return orderDirection_;
}

QString OrderExpression::Item::orderText() const
{
  return orderText_;
}

QString OrderExpression::Item::sql() const
{
  return sql_;
}

OrderExpression::OrderExpression()
  : items_()
{
}

OrderExpression::OrderExpression(const ItemList& items)
  : items_(items)
{
}

OrderExpression::ItemList OrderExpression::items() const
{
  return items_;
}

bool OrderExpression::isEmpty() const
{
  return items_.isEmpty();
}

void OrderExpression::addItem(const ItemPtr& item)
{
  items_.append(item);
}

void OrderExpression::removeItem(int index)
{
  items_.removeAt(index);
}  

void OrderExpression::clear()
{
  items_.clear();
}

bool OrderExpression::containsField(const FieldDescriptionPtr& field) const
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

QString OrderExpression::sql() const
{
  return buildSql(items_);
}

QString OrderExpression::buildSql(const ItemList& items)
{
  QString result;
  if (!items.isEmpty())
  {
    typedef ItemList::const_iterator iterator;          
    static const QString separator = QString::fromLatin1(", ");
    for (iterator i = items.begin(), end = items.end(); i != end; ++i)
    {            
      QString sql = (*i)->sql();
      if (!result.isEmpty())
      {
        result += separator;
      }
      result += sql;          
    }
  }  
  return result;
}

} // namespace chrono
} // namespace ma

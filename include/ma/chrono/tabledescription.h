//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_TABLEDESCRIPTION_H
#define MA_CHRONO_TABLEDESCRIPTION_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <QList>
#include <ma/chrono/fielddescription.h>

namespace ma {
namespace chrono {

class TableDescription
{
public:      
  typedef QList<FieldDescriptionPtr> FieldList;      

  TableDescription()
    : fields_()
  {
  }

  explicit TableDescription(const FieldList& fields)
    : fields_(fields)
  {
  }

  FieldList fields() const
  {
    return fields_;
  }

  bool isEmpty() const
  {
    return fields_.isEmpty();
  }

  void addField(const FieldDescriptionPtr& item)
  {
    fields_.append(item);
  }      

private:
  FieldList fields_;
}; // class TableDescription

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_TABLEDESCRIPTION_H

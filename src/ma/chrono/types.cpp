//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ma/chrono/types.h>

namespace ma {
namespace chrono {  

void registerGenericMetaTypes()
{
  qRegisterMetaType<OptionalQString>("ma::chrono::OptionalQString");
  qRegisterMetaType<OptionalQDate>("ma::chrono::OptionalQDate");
  qRegisterMetaType<OptionalQDateTime>("ma::chrono::OptionalQDateTime");
  qRegisterMetaType<OptionalInt>("ma::chrono::OptionalInt");
  //todo : check if OptionalInt is the same type as OptionalQInt32
  //qRegisterMetaType<OptionalQInt32>("ma::chrono::OptionalQInt32");
  //todo : check if OptionalInt is the same type as OptionalQInt64
  qRegisterMetaType<OptionalQInt64>("ma::chrono::OptionalQInt64");
}

} // namespace chrono
} // namespace ma

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_DBCONSTANTS_H
#define MA_CHRONO_DBCONSTANTS_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

namespace ma {
namespace chrono {    

enum Entity
{
  // Main entities
  userEntity = 1,
  contractEntity,
  insuranceEntity,
  paysheetEntity,
  ccdEntity,
  bolEntity,
  sbolEntity,
  lcreditEntity,
  ictEntity,
  icdocEntity,

  // Reference entities
  bankEntity,
  counterpartEntity,
  autoenterpriseEntity,
  residentEntity,
  ttermEntity,
  currencyEntity,
  countryEntity,    
  deppointEntity,
  entityCount,

  invoiceEntity,
  unknownEntity = 500,
}; // enum Entity
      
} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_DBCONSTANTS_H

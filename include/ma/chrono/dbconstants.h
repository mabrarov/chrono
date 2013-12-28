//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
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
} //namespace ma

#endif // MA_CHRONO_DBCONSTANTS_H

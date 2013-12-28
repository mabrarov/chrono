//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_MODEL_LCREDIT_H
#define MA_CHRONO_MODEL_LCREDIT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/config.hpp>
#include <QtGlobal>
#include <QString>
#include <QDateTime>
#include <ma/chrono/types.h>
#include <ma/chrono/model/lcredit_fwd.h>

namespace ma {
namespace chrono {
namespace model {

struct Lcredit
{
  BOOST_STATIC_CONSTANT(std::size_t, lcreditNumberMaxLen = 16);
  BOOST_STATIC_CONSTANT(std::size_t, remarkMaxLen        = 1024);

  qint64          lcreditId;                
  qint64          contractId;
  QString         lcreditNumber;
  QDate           lcreditDate;
  QDate           endDate;        
  OptionalQString remark;
  qint64          createUserId;
  qint64          updateUserId;        
  QDateTime       createTime;
  QDateTime       updateTime;
}; // struct Lcredit

} // namespace model    

} // namespace chrono

} //namespace ma

#endif // MA_CHRONO_MODEL_LCREDIT_H

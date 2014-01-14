//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_EXPORTDATALIST_H
#define MA_CHRONO_EXPORTDATALIST_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <utility>
#include <QString>
#include <QVariant>
#include <QList>

namespace ma {
namespace chrono {

typedef std::pair<QString, QVariant> ExportDataItem;
typedef QList<ExportDataItem> ExportDataList;

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_EXPORTDATALIST_H

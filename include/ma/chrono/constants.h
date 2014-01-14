//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_CONSTANTS_H
#define MA_CHRONO_CONSTANTS_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstdlib>
#include <QString>

namespace ma {
namespace chrono {

extern const int defaultDpiX;
extern const int defaultDpiY;

extern const std::size_t historyLength;

extern const QString chronoIconName;
extern const QString applicationQuitIconName;
extern const QString helpIconName;
extern const QString homeIconName;
extern const QString serviceIconName;
extern const QString settingsIconName;
extern const QString goBackIconName;
extern const QString goForwardIconName;
extern const QString refreshIconName;
extern const QString actionIconName;
extern const QString viewIconName;
extern const QString editIconName;
extern const QString createNewIconName;
extern const QString printIconName;
extern const QString summaryIconName;
extern const QString listIconName;
extern const QString filterIconName;
extern const QString usersIconName;
extern const QString databaseIconName;
extern const QString eventsIconName;
extern const QString generalSettingsIconName;
extern const QString userSettingsIconName;
extern const QString securitySettingsIconName;
extern const QString statisticsIconName;
extern const QString bookmarksOrganizeIconName;        
extern const QString deleteIconName;        
extern const QString exportIconName;        
extern const QString windowDuplicateIconName;        
extern const QString removeFilterIconName;        
extern const QString clearEditIconName;
extern const QString moveRightIconName;
extern const QString moveLeftIconName;
extern const QString moveUpIconName;
extern const QString moveDownIconName;

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_CONSTANTS_H

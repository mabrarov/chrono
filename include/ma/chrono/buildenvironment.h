//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_BUILD_ENVIRONMENT
#define MA_CHRONO_BUILD_ENVIRONMENT

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#if !defined(MA_CHRONO_BUILD_ENVIRONMENT_WINDOWS_VERSION_BIT)
#define MA_CHRONO_BUILD_ENVIRONMENT_WINDOWS_VERSION_BIT 64
#endif

#if !defined(MA_CHRONO_BUILD_ENVIRONMENT_OFFICE_VERSION)
#define MA_CHRONO_BUILD_ENVIRONMENT_OFFICE_VERSION 2013
#endif

#if !defined(MA_CHRONO_BUILD_ENVIRONMENT_OFFICE_VERSION_BIT)
#define MA_CHRONO_BUILD_ENVIRONMENT_OFFICE_VERSION_BIT 64
#endif

//#define MA_CHRONO_BUILD_ENVIRONMENT_DEMO
//#define MA_CHRONO_BUILD_ENVIRONMENT_DEMO_MAX_ROWS 10

#endif // MA_CHRONO_BUILD_ENVIRONMENT
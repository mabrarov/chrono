﻿# 
# Copyright (c) 2015 Marat Abrarov (abrarov@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 2.8.11)

# Prefix for all Firebird client relative public variables
if(NOT DEFINED FIREBIRD_CLIENT_PUBLIC_VAR_NS)
    set(FIREBIRD_CLIENT_PUBLIC_VAR_NS "FirebirdClient")
endif()

# Prefix for all Firebird relative internal variables
if(NOT DEFINED FIREBIRD_CLIENT_PRIVATE_VAR_NS)
    set(FIREBIRD_CLIENT_PRIVATE_VAR_NS "_${FIREBIRD_CLIENT_PUBLIC_VAR_NS}")
endif()

set(${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_FOUND FALSE)
set(${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_LIBRARIES )

if(DEFINED ${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_ROOT)
    if(NOT DEFINED ${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_INCLUDEDIR)
        set(${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_INCLUDEDIR "${${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_ROOT}/include")
    endif()
    if(NOT DEFINED ${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_LIBRARYDIR)
        set(${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_LIBRARYDIR "${${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_ROOT}/lib")
    endif()
endif()

if(NOT DEFINED ${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_INCLUDEDIR)
    set(${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_INCLUDEDIR )
endif()
if(NOT DEFINED ${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_LIBRARYDIR)
    set(${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_LIBRARYDIR )
endif()

if(NOT DEFINED ${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_INCLUDEDIR)
    find_path(${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_INCLUDEDIR 
        NAMES "ibase.h"
        DOC "Header directory for client library of Firebird")
endif()

find_library(${FIREBIRD_CLIENT_PRIVATE_VAR_NS}_LIB_FBCLIENT
    NAMES "fbclient" "fbclient_ms"
    HINTS "${${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_LIBRARYDIR}"
    DOC "Client library of Firebird")

if(${FIREBIRD_CLIENT_PRIVATE_VAR_NS}_LIB_FBCLIENT)
    set(${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_FOUND TRUE)
endif()

if(${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_FOUND)
    add_library(Firebird::Client STATIC IMPORTED)

    if(${FIREBIRD_CLIENT_PRIVATE_VAR_NS}_LIB_FBCLIENT)
        set_property(TARGET Firebird::Client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
        set_target_properties(Firebird::Client PROPERTIES IMPORTED_LOCATION_RELEASE 
            "${${FIREBIRD_CLIENT_PRIVATE_VAR_NS}_LIB_FBCLIENT}")
        list(APPEND ${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_LIBRARIES 
            optimized ${${FIREBIRD_CLIENT_PRIVATE_VAR_NS}_LIB_FBCLIENT})
    endif()
endif()

include(FindPackageHandleStandardArgs)
if(${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_FIND_REQUIRED AND NOT ${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_FIND_QUIETLY)
    find_package_handle_standard_args(${FIREBIRD_CLIENT_PUBLIC_VAR_NS}
        FOUND_VAR ${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_FOUND
        REQUIRED_VARS ${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_LIBRARIES ${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_INCLUDEDIR)
else()
    find_package_handle_standard_args(${FIREBIRD_CLIENT_PUBLIC_VAR_NS}
        FOUND_VAR ${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_FOUND
        REQUIRED_VARS ${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_LIBRARIES ${FIREBIRD_CLIENT_PUBLIC_VAR_NS}_INCLUDEDIR
        FAIL_MESSAGE "Firebird client library not found")
endif()

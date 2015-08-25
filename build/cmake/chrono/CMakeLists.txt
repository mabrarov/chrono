# 
# Copyright (c) 2015 Marat Abrarov (abrarov@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 2.8.11)
project(chrono)

# Just determine version of Boost C++ Libraries
find_package(Boost REQUIRED)

# Build list of used Boost libraries depending on Boost version 
# (use chono and timer if available)
set(BOOST_COMPONENTS "system;thread;date_time")
if(NOT(${Boost_VERSION} LESS 104700))
    set(BOOST_COMPONENTS "${BOOST_COMPONENTS};chrono")
    set(BOOST_HAS_CHRONO TRUE)
else()
    set(BOOST_HAS_CHRONO FALSE)
endif()
if(NOT(${Boost_VERSION} LESS 104800))
    set(BOOST_HAS_TIMER TRUE)
else()
    set(BOOST_HAS_TIMER FALSE)
endif()

# Attach used Boost libraries
find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

# Determine major version of Qt to use (Qt 5 is predeferred)
if(NOT DEFINED CHRONO_USE_QT5)
    set(CHRONO_USE_QT5 TRUE)
    find_package(Qt5Widgets QUIET)
    if(NOT Qt5Widgets_FOUND)
        set(CHRONO_USE_QT5 FALSE)
    endif()
endif()

# Find required Qt libraries
if(CHRONO_USE_QT5)
    find_package(Qt5Widgets       REQUIRED)
    find_package(Qt5Gui           REQUIRED)
    find_package(Qt5Core          REQUIRED)
    find_package(Qt5Sql           REQUIRED)
    find_package(Qt5Svg           REQUIRED)
    find_package(Qt5LinguistTools QUIET)
    set(CHRONO_QT_LIBRARIES "Qt5::Core;Qt5::Gui;Qt5::Widgets;Qt5::Sql;Qt5::Svg")
else()
    find_package(Qt4 REQUIRED QtCore QtGui QtSvg QtSql)
    set(CHRONO_QT_LIBRARIES "Qt4::QtCore;Qt4::QtGui;Qt4::QtSvg;Qt4::QtSql")
endif()

set(HEADERS_DIR   "${PROJECT_SOURCE_DIR}/../../../include")
set(SOURCES_DIR   "${PROJECT_SOURCE_DIR}/../../../src")
set(RESOURCES_DIR "${PROJECT_SOURCE_DIR}/../../../resources")

set(MOC_HEADERS
    "${HEADERS_DIR}/ma/chrono/addressbar.h"
    "${HEADERS_DIR}/ma/chrono/amountdelegate.h"
    "${HEADERS_DIR}/ma/chrono/autoenterprisecardpage.h"
    "${HEADERS_DIR}/ma/chrono/autoenterpriselistpage.h"
    "${HEADERS_DIR}/ma/chrono/autoenterprisetablemodel.h"
    "${HEADERS_DIR}/ma/chrono/bankcardpage.h"
    "${HEADERS_DIR}/ma/chrono/banklistpage.h"
    "${HEADERS_DIR}/ma/chrono/banktablemodel.h"
    "${HEADERS_DIR}/ma/chrono/basictablemodel.h"
    "${HEADERS_DIR}/ma/chrono/bolcardpage.h"
    "${HEADERS_DIR}/ma/chrono/bollistpage.h"
    "${HEADERS_DIR}/ma/chrono/boltablemodel.h"
    "${HEADERS_DIR}/ma/chrono/breadcrumbbar.h"
    "${HEADERS_DIR}/ma/chrono/cardpage.h"
    "${HEADERS_DIR}/ma/chrono/cardtaskwindow.h"
    "${HEADERS_DIR}/ma/chrono/ccdcardpage.h"
    "${HEADERS_DIR}/ma/chrono/ccdlistpage.h"
    "${HEADERS_DIR}/ma/chrono/ccdtablemodel.h"
    "${HEADERS_DIR}/ma/chrono/commandbar.h"
    "${HEADERS_DIR}/ma/chrono/commandbutton.h"
    "${HEADERS_DIR}/ma/chrono/commanddialog.h"
    "${HEADERS_DIR}/ma/chrono/conditiondialog.h"
    "${HEADERS_DIR}/ma/chrono/contractbollistpage.h"
    "${HEADERS_DIR}/ma/chrono/contractboltablemodel.h"
    "${HEADERS_DIR}/ma/chrono/contractcardpage.h"
    "${HEADERS_DIR}/ma/chrono/contractccdlistpage.h"
    "${HEADERS_DIR}/ma/chrono/contractccdtablemodel.h"
    "${HEADERS_DIR}/ma/chrono/contractinsurancelistpage.h"
    "${HEADERS_DIR}/ma/chrono/contractinsurancetablemodel.h"
    "${HEADERS_DIR}/ma/chrono/contractlistpage.h"
    "${HEADERS_DIR}/ma/chrono/contractpaysheetlistpage.h"
    "${HEADERS_DIR}/ma/chrono/contractpaysheettablemodel.h"
    "${HEADERS_DIR}/ma/chrono/contractrelatedlistpage.h"
    "${HEADERS_DIR}/ma/chrono/contractsbollistpage.h"
    "${HEADERS_DIR}/ma/chrono/contractsboltablemodel.h"
    "${HEADERS_DIR}/ma/chrono/contracttablemodel.h"
    "${HEADERS_DIR}/ma/chrono/counterpartcardpage.h"
    "${HEADERS_DIR}/ma/chrono/counterpartlistpage.h"
    "${HEADERS_DIR}/ma/chrono/counterparttablemodel.h"
    "${HEADERS_DIR}/ma/chrono/countrycardpage.h"
    "${HEADERS_DIR}/ma/chrono/countrylistpage.h"
    "${HEADERS_DIR}/ma/chrono/countrynumericcodedelegate.h"
    "${HEADERS_DIR}/ma/chrono/countrytablemodel.h"
    "${HEADERS_DIR}/ma/chrono/currencycardpage.h"
    "${HEADERS_DIR}/ma/chrono/currencylistpage.h"
    "${HEADERS_DIR}/ma/chrono/currencynumericcodedelegate.h"
    "${HEADERS_DIR}/ma/chrono/currencytablemodel.h"
    "${HEADERS_DIR}/ma/chrono/databaseconnectdialog.h"
    "${HEADERS_DIR}/ma/chrono/databasemodel.h"
    "${HEADERS_DIR}/ma/chrono/datapage.h"
    "${HEADERS_DIR}/ma/chrono/deppointcardpage.h"
    "${HEADERS_DIR}/ma/chrono/deppointlistpage.h"
    "${HEADERS_DIR}/ma/chrono/deppointtablemodel.h"
    "${HEADERS_DIR}/ma/chrono/filterdialog.h"
    "${HEADERS_DIR}/ma/chrono/icdoccardpage.h"
    "${HEADERS_DIR}/ma/chrono/icdoclistpage.h"
    "${HEADERS_DIR}/ma/chrono/icdoctablemodel.h"
    "${HEADERS_DIR}/ma/chrono/ictcardpage.h"
    "${HEADERS_DIR}/ma/chrono/ictlistpage.h"
    "${HEADERS_DIR}/ma/chrono/icttablemodel.h"
    "${HEADERS_DIR}/ma/chrono/insurancecardpage.h"
    "${HEADERS_DIR}/ma/chrono/insurancelistpage.h"
    "${HEADERS_DIR}/ma/chrono/insurancetablemodel.h"
    "${HEADERS_DIR}/ma/chrono/lcreditcardpage.h"
    "${HEADERS_DIR}/ma/chrono/lcreditlistpage.h"
    "${HEADERS_DIR}/ma/chrono/lcredittablemodel.h"
    "${HEADERS_DIR}/ma/chrono/listpage.h"
    "${HEADERS_DIR}/ma/chrono/listselectdialog.h"
    "${HEADERS_DIR}/ma/chrono/listtaskwindow.h"
    "${HEADERS_DIR}/ma/chrono/navigationbutton.h"
    "${HEADERS_DIR}/ma/chrono/navigationmodel.h"
    "${HEADERS_DIR}/ma/chrono/navigationwindow.h"
    "${HEADERS_DIR}/ma/chrono/page.h"
    "${HEADERS_DIR}/ma/chrono/paysheetcardpage.h"
    "${HEADERS_DIR}/ma/chrono/paysheetlistpage.h"
    "${HEADERS_DIR}/ma/chrono/paysheettablemodel.h"
    "${HEADERS_DIR}/ma/chrono/ratedelegate.h"
    "${HEADERS_DIR}/ma/chrono/residentcardpage.h"
    "${HEADERS_DIR}/ma/chrono/residentlistpage.h"
    "${HEADERS_DIR}/ma/chrono/residenttablemodel.h"
    "${HEADERS_DIR}/ma/chrono/sbolcardpage.h"
    "${HEADERS_DIR}/ma/chrono/sbollistpage.h"
    "${HEADERS_DIR}/ma/chrono/sboltablemodel.h"
    "${HEADERS_DIR}/ma/chrono/sortdialog.h"
    "${HEADERS_DIR}/ma/chrono/summarypage.h"
    "${HEADERS_DIR}/ma/chrono/taskwindow.h"
    "${HEADERS_DIR}/ma/chrono/taskwindowmanager.h"
    "${HEADERS_DIR}/ma/chrono/ttermcardpage.h"
    "${HEADERS_DIR}/ma/chrono/ttermlistpage.h"
    "${HEADERS_DIR}/ma/chrono/ttermtablemodel.h"
    "${HEADERS_DIR}/ma/chrono/usercardpage.h"
    "${HEADERS_DIR}/ma/chrono/userlistpage.h"
    "${HEADERS_DIR}/ma/chrono/usertablemodel.h")

set(UIC_FORMS
    "${SOURCES_DIR}/ma/chrono/autoenterprisecardpage.ui"
    "${SOURCES_DIR}/ma/chrono/bankcardpage.ui"
    "${SOURCES_DIR}/ma/chrono/bolcardpage.ui"
    "${SOURCES_DIR}/ma/chrono/ccdcardpage.ui"
    "${SOURCES_DIR}/ma/chrono/conditiondialog.ui"
    "${SOURCES_DIR}/ma/chrono/contractcardpage.ui"
    "${SOURCES_DIR}/ma/chrono/counterpartcardpage.ui"
    "${SOURCES_DIR}/ma/chrono/countrycardpage.ui"
    "${SOURCES_DIR}/ma/chrono/currencycardpage.ui"
    "${SOURCES_DIR}/ma/chrono/databaseconnectdialog.ui"
    "${SOURCES_DIR}/ma/chrono/deppointcardpage.ui"
    "${SOURCES_DIR}/ma/chrono/filterdialog.ui"
    "${SOURCES_DIR}/ma/chrono/icdoccardpage.ui"
    "${SOURCES_DIR}/ma/chrono/ictcardpage.ui"
    "${SOURCES_DIR}/ma/chrono/insurancecardpage.ui"
    "${SOURCES_DIR}/ma/chrono/lcreditcardpage.ui"
    "${SOURCES_DIR}/ma/chrono/paysheetcardpage.ui"
    "${SOURCES_DIR}/ma/chrono/residentcardpage.ui"
    "${SOURCES_DIR}/ma/chrono/sbolcardpage.ui"
    "${SOURCES_DIR}/ma/chrono/sortdialog.ui"
    "${SOURCES_DIR}/ma/chrono/summarypage.ui"
    "${SOURCES_DIR}/ma/chrono/ttermcardpage.ui"
    "${SOURCES_DIR}/ma/chrono/usercardpage.ui")

set(RCC_RESOURCES
    "${RESOURCES_DIR}/chrono.qrc")

set(TRANSLATION_SOURCES
    "${RESOURCES_DIR}/chrono_ru.ts")

set(HEADERS
    "${HEADERS_DIR}/ma/chrono/actionutility.h"    
    "${HEADERS_DIR}/ma/chrono/addressbar_fwd.h"
    "${HEADERS_DIR}/ma/chrono/autoenterprisetablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/banktablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/basicfiltercondition.h"
    "${HEADERS_DIR}/ma/chrono/basictablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/binaryfiltercondition.h"
    "${HEADERS_DIR}/ma/chrono/boltablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/breadcrumbbar_fwd.h"
    "${HEADERS_DIR}/ma/chrono/buildenvironment.h"
    "${HEADERS_DIR}/ma/chrono/cardpage_fwd.h"
    "${HEADERS_DIR}/ma/chrono/cardtaskwindow_fwd.h"
    "${HEADERS_DIR}/ma/chrono/ccdtablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/commandbar_fwd.h"
    "${HEADERS_DIR}/ma/chrono/commandbutton_fwd.h"
    "${HEADERS_DIR}/ma/chrono/conditiondialog_fwd.h"
    "${HEADERS_DIR}/ma/chrono/constants.h"
    "${HEADERS_DIR}/ma/chrono/contractbollistpage_fwd.h"
    "${HEADERS_DIR}/ma/chrono/contractboltablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/contractccdlistpage_fwd.h"
    "${HEADERS_DIR}/ma/chrono/contractccdtablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/contractinsurancelistpage_fwd.h"
    "${HEADERS_DIR}/ma/chrono/contractinsurancetablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/contractpaysheetlistpage_fwd.h"
    "${HEADERS_DIR}/ma/chrono/contractpaysheettablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/contractsbollistpage_fwd.h"
    "${HEADERS_DIR}/ma/chrono/contractsboltablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/contracttablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/counterparttablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/countrytablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/currencytablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/databaseconnectdialog_fwd.h"
    "${HEADERS_DIR}/ma/chrono/databasemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/dateutility.h"
    "${HEADERS_DIR}/ma/chrono/dbconstants.h"
    "${HEADERS_DIR}/ma/chrono/deppointtablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/excelexporter.h"
    "${HEADERS_DIR}/ma/chrono/exportdatalist.h"
    "${HEADERS_DIR}/ma/chrono/fielddescription.h"
    "${HEADERS_DIR}/ma/chrono/filterdialog_fwd.h"
    "${HEADERS_DIR}/ma/chrono/filterexpression.h"
    "${HEADERS_DIR}/ma/chrono/generator.h"
    "${HEADERS_DIR}/ma/chrono/generator_fwd.h"
    "${HEADERS_DIR}/ma/chrono/historylist.h"
    "${HEADERS_DIR}/ma/chrono/icdoctablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/icttablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/insurancetablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/lcredittablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/listpage_fwd.h"
    "${HEADERS_DIR}/ma/chrono/listtaskwindow_fwd.h"
    "${HEADERS_DIR}/ma/chrono/localeutility.h"
    "${HEADERS_DIR}/ma/chrono/modelutility.h"
    "${HEADERS_DIR}/ma/chrono/navigationbutton_fwd.h"
    "${HEADERS_DIR}/ma/chrono/navigationmodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/onefieldbasedfiltercondition.h"
    "${HEADERS_DIR}/ma/chrono/orderexpression.h"
    "${HEADERS_DIR}/ma/chrono/page_fwd.h"
    "${HEADERS_DIR}/ma/chrono/paysheettablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/residenttablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/resourcemanager.h"
    "${HEADERS_DIR}/ma/chrono/resourcemanager_fwd.h"
    "${HEADERS_DIR}/ma/chrono/sboltablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/sortdialog_fwd.h"
    "${HEADERS_DIR}/ma/chrono/sqlutility.h"
    "${HEADERS_DIR}/ma/chrono/tabledescription.h"
    "${HEADERS_DIR}/ma/chrono/taskwindowmanager_fwd.h"
    "${HEADERS_DIR}/ma/chrono/taskwindow_fwd.h"
    "${HEADERS_DIR}/ma/chrono/ttermtablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/types.h"
    "${HEADERS_DIR}/ma/chrono/types_fwd.h"
    "${HEADERS_DIR}/ma/chrono/unaryfiltercondition.h"
    "${HEADERS_DIR}/ma/chrono/userdefinedfilter.h"
    "${HEADERS_DIR}/ma/chrono/userdefinedorder.h"
    "${HEADERS_DIR}/ma/chrono/usertablemodel_fwd.h"
    "${HEADERS_DIR}/ma/chrono/widgetutility.h"
    "${HEADERS_DIR}/ma/chrono/model/autoenterprise.h"
    "${HEADERS_DIR}/ma/chrono/model/autoenterprise_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/bank.h"
    "${HEADERS_DIR}/ma/chrono/model/bank_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/bol.h"
    "${HEADERS_DIR}/ma/chrono/model/bol_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/ccd.h"
    "${HEADERS_DIR}/ma/chrono/model/ccd_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/contract.h"
    "${HEADERS_DIR}/ma/chrono/model/contract_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/counterpart.h"
    "${HEADERS_DIR}/ma/chrono/model/counterpart_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/country.h"
    "${HEADERS_DIR}/ma/chrono/model/country_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/currency.h"
    "${HEADERS_DIR}/ma/chrono/model/currencyamount.h"
    "${HEADERS_DIR}/ma/chrono/model/currencyamount_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/currency_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/decimalamount.h"
    "${HEADERS_DIR}/ma/chrono/model/deppoint.h"
    "${HEADERS_DIR}/ma/chrono/model/deppoint_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/icdoc.h"
    "${HEADERS_DIR}/ma/chrono/model/icdoc_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/ict.h"
    "${HEADERS_DIR}/ma/chrono/model/ict_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/insurance.h"
    "${HEADERS_DIR}/ma/chrono/model/insurance_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/lcredit.h"
    "${HEADERS_DIR}/ma/chrono/model/lcredit_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/paysheet.h"
    "${HEADERS_DIR}/ma/chrono/model/paysheet_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/rateamount.h"
    "${HEADERS_DIR}/ma/chrono/model/rateamount_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/resident.h"
    "${HEADERS_DIR}/ma/chrono/model/resident_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/sbol.h"
    "${HEADERS_DIR}/ma/chrono/model/sbol_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/tterm.h"
    "${HEADERS_DIR}/ma/chrono/model/tterm_fwd.h"
    "${HEADERS_DIR}/ma/chrono/model/user.h"
    "${HEADERS_DIR}/ma/chrono/model/user_fwd.h"
    ${MOC_HEADERS})

set(MP_COMPAT_SOURCES
    "${SOURCES_DIR}/main.cpp"
    "${SOURCES_DIR}/ma/chrono/types.cpp"
    "${SOURCES_DIR}/ma/chrono/actionutility.cpp"
    "${SOURCES_DIR}/ma/chrono/addressbar.cpp"
    "${SOURCES_DIR}/ma/chrono/amountdelegate.cpp"
    "${SOURCES_DIR}/ma/chrono/autoenterprisecardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/autoenterpriselistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/autoenterprisetablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/bankcardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/banklistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/banktablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/basictablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/binaryfiltercondition.cpp"
    "${SOURCES_DIR}/ma/chrono/bolcardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/bollistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/boltablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/breadcrumbbar.cpp"
    "${SOURCES_DIR}/ma/chrono/cardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/cardtaskwindow.cpp"
    "${SOURCES_DIR}/ma/chrono/ccdcardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/ccdlistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/ccdtablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/commandbar.cpp"
    "${SOURCES_DIR}/ma/chrono/commandbutton.cpp"
    "${SOURCES_DIR}/ma/chrono/commanddialog.cpp"
    "${SOURCES_DIR}/ma/chrono/conditiondialog.cpp"
    "${SOURCES_DIR}/ma/chrono/constants.cpp"
    "${SOURCES_DIR}/ma/chrono/contractbollistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/contractboltablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/contractcardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/contractccdlistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/contractccdtablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/contractinsurancelistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/contractinsurancetablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/contractlistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/contractpaysheetlistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/contractpaysheettablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/contractrelatedlistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/contractsbollistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/contractsboltablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/contracttablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/counterpartcardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/counterpartlistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/counterparttablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/countrycardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/countrylistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/countrynumericcodedelegate.cpp"
    "${SOURCES_DIR}/ma/chrono/countrytablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/currencycardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/currencylistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/currencynumericcodedelegate.cpp"
    "${SOURCES_DIR}/ma/chrono/currencytablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/databaseconnectdialog.cpp"
    "${SOURCES_DIR}/ma/chrono/databasemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/datapage.cpp"
    "${SOURCES_DIR}/ma/chrono/dateutility.cpp"
    "${SOURCES_DIR}/ma/chrono/dbconstants.cpp"
    "${SOURCES_DIR}/ma/chrono/deppointcardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/deppointlistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/deppointtablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/filterdialog.cpp"
    "${SOURCES_DIR}/ma/chrono/generator.cpp"
    "${SOURCES_DIR}/ma/chrono/icdoccardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/icdoclistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/icdoctablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/ictcardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/ictlistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/icttablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/insurancecardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/insurancelistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/insurancetablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/lcreditcardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/lcreditlistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/lcredittablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/listpage.cpp"
    "${SOURCES_DIR}/ma/chrono/listselectdialog.cpp"
    "${SOURCES_DIR}/ma/chrono/listtaskwindow.cpp"
    "${SOURCES_DIR}/ma/chrono/localeutility.cpp"
    "${SOURCES_DIR}/ma/chrono/modelutility.cpp"
    "${SOURCES_DIR}/ma/chrono/navigationbutton.cpp"
    "${SOURCES_DIR}/ma/chrono/navigationmodel.cpp"
    "${SOURCES_DIR}/ma/chrono/navigationwindow.cpp"
    "${SOURCES_DIR}/ma/chrono/orderexpression.cpp"
    "${SOURCES_DIR}/ma/chrono/page.cpp"
    "${SOURCES_DIR}/ma/chrono/paysheetcardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/paysheetlistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/paysheettablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/ratedelegate.cpp"
    "${SOURCES_DIR}/ma/chrono/residentcardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/residentlistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/residenttablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/resourcemanager.cpp"
    "${SOURCES_DIR}/ma/chrono/sbolcardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/sbollistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/sboltablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/sortdialog.cpp"
    "${SOURCES_DIR}/ma/chrono/summarypage.cpp"
    "${SOURCES_DIR}/ma/chrono/taskwindow.cpp"
    "${SOURCES_DIR}/ma/chrono/taskwindowmanager.cpp"
    "${SOURCES_DIR}/ma/chrono/ttermcardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/ttermlistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/ttermtablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/unaryfiltercondition.cpp"
    "${SOURCES_DIR}/ma/chrono/usercardpage.cpp"
    "${SOURCES_DIR}/ma/chrono/userlistpage.cpp"
    "${SOURCES_DIR}/ma/chrono/usertablemodel.cpp"
    "${SOURCES_DIR}/ma/chrono/widgetutility.cpp"
    "${SOURCES_DIR}/ma/chrono/model/decimalamount.cpp")

# Some source files doesn't support multithreaded compilation 
list(APPEND SOURCES ${MP_COMPAT_SOURCES} ${UIC_FORMS}
    "${SOURCES_DIR}/ma/chrono/excelexporter.cpp")

# Windows specific source files
if(WIN32)
    list(APPEND SOURCES "${RESOURCES_DIR}/chrono.rc")
endif()

if((NOT CHRONO_USE_QT5) OR Qt5LinguistTools_FOUND)
    list(APPEND SOURCES ${TRANSLATION_SOURCES})
endif()

set(LIBRARIES "${Boost_LIBRARIES};${CHRONO_QT_LIBRARIES}")

if(CHRONO_USE_QT5)
    # Determine linkage type of Qt libraries (via linkage type of Qt Core library)
    get_property(Qt5_Core_TYPE TARGET Qt5::Core PROPERTY TYPE)
    set(QT5_IS_STATIC false)
    if(${Qt5_Core_TYPE} STREQUAL "STATIC_LIBRARY")
        set(QT5_IS_STATIC true)
    endif()

    # Statically linked Qt requires:
    #     All libraries used by Qt internally to be linked explicitly to the target.
    #     Used Qt plugins to be linked explicitly to the target.
    if(QT5_IS_STATIC)
        # Qt CMake files use IMPORTED_LINK_DEPENDENT_LIBRARIES instead of INTERFACE_LINK_LIBRARIES for OpenGL dependencies.
        # So we need to add those transitive (in case of static linkage) dependencies explicitly.
        set(LIBRARIES "${LIBRARIES};${Qt5Gui_EGL_LIBRARIES};${Qt5Gui_OPENGL_LIBRARIES}")

        # Add required platform integration plugins
        if(WIN32)            
            set(LIBRARIES "${LIBRARIES};Qt5::QWindowsIntegrationPlugin")
        else()
            # todo:
            # add support for the rest of platforms supported by Qt
            # by linking with platform specific plugin
            # (add platfrom integration plugin to LIBRARIES)
        endif()

        # Add Qt SVG and Qt SQL plugins
        set(LIBRARIES "${LIBRARIES};Qt5::QSvgPlugin;Qt5::QSvgIconPlugin;Qt5::QIBaseDriverPlugin")

        # Some Qt libraries have no cmake files so wee need to search them ourselves
        # Refer to custom CMake modules:
        #     FindICU.cmake
        #     FindQt5PlatformSupport.cmake
        #     FindQt5HarfBuzzNG.cmake
        #     FindQt5FreeType.cmake
        #     FindQt5PCRE.cmake

        find_package(Qt5PlatformSupport REQUIRED)
        set(LIBRARIES "${LIBRARIES};Qt5::PlatformSupport")

        find_package(ICU REQUIRED COMPONENTS i18n uc data)
        set(LIBRARIES "${LIBRARIES};${ICU_LIBRARIES}")

        # Starting from Qt 5.4.0 HarfBuzz-NG library is included (at least Windows version of Qt 5.4.0)
        if(WIN32 AND NOT(${Qt5Gui_VERSION} VERSION_LESS "5.4.0"))
            find_package(Qt5HarfBuzzNG QUIET)
            if(Qt5HarfBuzzNG_FOUND)
                set(LIBRARIES "${LIBRARIES};Qt5::HarfBuzzNG")
            endif()
        endif()

        # Starting from Qt 5.5.0 FreeType library is used by QtGui
        if(WIN32 AND NOT(${Qt5Gui_VERSION} VERSION_LESS "5.5.0"))
            find_package(Qt5FreeType QUIET)
            if(Qt5FreeType_FOUND)
                set(LIBRARIES "${LIBRARIES};Qt5::FreeType")
            endif()
        endif()

        # Starting from Qt 5.5.0 PCRE library is used by QtCore
        if(WIN32 AND NOT(${Qt5Core_VERSION} VERSION_LESS "5.5.0"))
            find_package(Qt5PCRE QUIET)
            if(Qt5PCRE_FOUND)
                set(LIBRARIES "${LIBRARIES};Qt5::PCRE")
            endif()
        endif()

        # Firebird client librray
        # Refer to custom CMake modules: 
        #     FindFirebirdClient.cmake

        find_package(FirebirdClient REQUIRED)
        set(LIBRARIES "${LIBRARIES};Firebird::Client")
    endif()
else()
    # Statically linked Qt requires:
    #     All libraries used by Qt internally to be linked explicitly to the target.
    #     Used Qt plugins to be linked explicitly to the target.
    if(QT_IS_STATIC)
        # Add Qt SVG and Qt SQL plugins
        set(LIBRARIES "${LIBRARIES};Qt4::qsvg;Qt4::qsvgicon;Qt4::qsqlibase")

        # Firebird client librray
        # Refer to custom CMake modules:
        #     FindFirebirdClient.cmake

        find_package(FirebirdClient REQUIRED)
        set(LIBRARIES "${LIBRARIES};Firebird::Client")
    endif()
endif()

# Required Windows libraries
if(WIN32)
    set(LIBRARIES "${LIBRARIES};comdlg32;uuid;ole32;oleaut32;advapi32;imm32;winmm;winspool;ws2_32.lib")
endif()

# Boost chrono requires rt library on *nix
if(UNIX AND BOOST_HAS_CHRONO)
    set(LIBRARIES "${LIBRARIES};rt")
endif()

set(LRELEASE_TRANSLATIONS )

# Turn on parallel compilation of source files for MSVC.
# This otion speeds up overall build time
if(MSVC)
    if(NOT (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "16"))
        set_source_files_properties(
            ${MP_COMPAT_SOURCES}
            ${MOC_GENERATED_SOURCES}
            ${RCC_GENERATED_SOURCES} 
            PROPERTIES COMPILE_FLAGS /MP)
    endif()
endif()

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${HEADERS_DIR})

# Wrap Qt generated sources.
if(CHRONO_USE_QT5)
    qt5_wrap_cpp(MOC_GENERATED_SOURCES ${MOC_HEADERS})
    qt5_wrap_ui(UIC_GENERATED_HEADERS ${UIC_FORMS})
    qt5_add_resources(RCC_GENERATED_SOURCES ${RCC_RESOURCES})
    if(Qt5LinguistTools_FOUND)
        qt5_add_translation(LRELEASE_TRANSLATIONS ${TRANSLATION_SOURCES})
    endif()
else()
    qt4_wrap_cpp(MOC_GENERATED_SOURCES ${MOC_HEADERS})
    qt4_wrap_ui(UIC_GENERATED_HEADERS ${UIC_FORMS})
    qt4_add_resources(RCC_GENERATED_SOURCES ${RCC_RESOURCES})
    qt4_add_translation(LRELEASE_TRANSLATIONS ${TRANSLATION_SOURCES})
endif()

# Required for Qt UIC
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(chrono WIN32 
    ${HEADERS} ${SOURCES}
    ${MOC_GENERATED_SOURCES} ${RCC_GENERATED_SOURCES} ${UIC_GENERATED_HEADERS} ${LRELEASE_TRANSLATIONS})
target_link_libraries(chrono ${LIBRARIES})

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
    set(Qt_LIBRARIES "Qt5::Core;Qt5::Gui;Qt5::Widgets;Qt5::Sql;Qt5::Svg")
else()
    find_package(Qt4 REQUIRED QtCore QtGui QtSvg QtSql)
    set(Qt_LIBRARIES "Qt4::QtCore;Qt4::QtGui;Qt4::QtSvg;Qt4::QtSql")
endif()

set(PROJECT_BASE_DIR       "${PROJECT_SOURCE_DIR}/../../..")
set(SOURCES_DIR            "${PROJECT_BASE_DIR}/src")
set(CPP_HEADERS_DIR        "${PROJECT_BASE_DIR}/include")
set(RESOURCES_DIR          "${PROJECT_BASE_DIR}/resources")
set(TRANSLATION_OUTPUT_DIR "${RESOURCES_DIR}/ma/chrono/translation")

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

set(QRC_RESOURCES
    "${RESOURCES_DIR}/chrono.qrc")

set(TRANSLATION_SOURCES
    "${RESOURCES_DIR}/chrono_ru.ts")

# QRC resource file expects compiled translation being located at resources dir
set_source_files_properties(${TRANSLATION_SOURCES} PROPERTIES OUTPUT_LOCATION "${TRANSLATION_OUTPUT_DIR}")

set(MOC_HEADERS
    "${CPP_HEADERS_DIR}/ma/chrono/addressbar.h"
    "${CPP_HEADERS_DIR}/ma/chrono/amountdelegate.h"
    "${CPP_HEADERS_DIR}/ma/chrono/autoenterprisecardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/autoenterpriselistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/autoenterprisetablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/bankcardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/banklistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/banktablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/basictablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/bolcardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/bollistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/boltablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/breadcrumbbar.h"
    "${CPP_HEADERS_DIR}/ma/chrono/cardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/cardtaskwindow.h"
    "${CPP_HEADERS_DIR}/ma/chrono/ccdcardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/ccdlistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/ccdtablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/commandbar.h"
    "${CPP_HEADERS_DIR}/ma/chrono/commandbutton.h"
    "${CPP_HEADERS_DIR}/ma/chrono/commanddialog.h"
    "${CPP_HEADERS_DIR}/ma/chrono/conditiondialog.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractbollistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractboltablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractcardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractccdlistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractccdtablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractinsurancelistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractinsurancetablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractlistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractpaysheetlistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractpaysheettablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractrelatedlistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractsbollistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractsboltablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contracttablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/counterpartcardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/counterpartlistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/counterparttablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/countrycardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/countrylistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/countrynumericcodedelegate.h"
    "${CPP_HEADERS_DIR}/ma/chrono/countrytablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/currencycardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/currencylistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/currencynumericcodedelegate.h"
    "${CPP_HEADERS_DIR}/ma/chrono/currencytablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/databaseconnectdialog.h"
    "${CPP_HEADERS_DIR}/ma/chrono/databasemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/datapage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/deppointcardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/deppointlistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/deppointtablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/filterdialog.h"
    "${CPP_HEADERS_DIR}/ma/chrono/icdoccardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/icdoclistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/icdoctablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/ictcardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/ictlistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/icttablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/insurancecardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/insurancelistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/insurancetablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/lcreditcardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/lcreditlistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/lcredittablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/listpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/listselectdialog.h"
    "${CPP_HEADERS_DIR}/ma/chrono/listtaskwindow.h"
    "${CPP_HEADERS_DIR}/ma/chrono/navigationbutton.h"
    "${CPP_HEADERS_DIR}/ma/chrono/navigationmodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/navigationwindow.h"
    "${CPP_HEADERS_DIR}/ma/chrono/page.h"
    "${CPP_HEADERS_DIR}/ma/chrono/paysheetcardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/paysheetlistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/paysheettablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/ratedelegate.h"
    "${CPP_HEADERS_DIR}/ma/chrono/residentcardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/residentlistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/residenttablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/sbolcardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/sbollistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/sboltablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/sortdialog.h"
    "${CPP_HEADERS_DIR}/ma/chrono/summarypage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/taskwindow.h"
    "${CPP_HEADERS_DIR}/ma/chrono/taskwindowmanager.h"
    "${CPP_HEADERS_DIR}/ma/chrono/ttermcardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/ttermlistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/ttermtablemodel.h"
    "${CPP_HEADERS_DIR}/ma/chrono/usercardpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/userlistpage.h"
    "${CPP_HEADERS_DIR}/ma/chrono/usertablemodel.h")

set(CPP_HEADERS
    "${CPP_HEADERS_DIR}/ma/chrono/actionutility.h"    
    "${CPP_HEADERS_DIR}/ma/chrono/addressbar_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/autoenterprisetablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/banktablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/basicfiltercondition.h"
    "${CPP_HEADERS_DIR}/ma/chrono/basictablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/binaryfiltercondition.h"
    "${CPP_HEADERS_DIR}/ma/chrono/boltablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/breadcrumbbar_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/buildenvironment.h"
    "${CPP_HEADERS_DIR}/ma/chrono/cardpage_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/cardtaskwindow_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/ccdtablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/commandbar_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/commandbutton_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/conditiondialog_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/constants.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractbollistpage_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractboltablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractccdlistpage_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractccdtablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractinsurancelistpage_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractinsurancetablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractpaysheetlistpage_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractpaysheettablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractsbollistpage_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contractsboltablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/contracttablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/counterparttablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/countrytablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/currencytablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/databaseconnectdialog_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/databasemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/dateutility.h"
    "${CPP_HEADERS_DIR}/ma/chrono/dbconstants.h"
    "${CPP_HEADERS_DIR}/ma/chrono/deppointtablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/excelexporter.h"
    "${CPP_HEADERS_DIR}/ma/chrono/exportdatalist.h"
    "${CPP_HEADERS_DIR}/ma/chrono/fielddescription.h"
    "${CPP_HEADERS_DIR}/ma/chrono/filterdialog_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/filterexpression.h"
    "${CPP_HEADERS_DIR}/ma/chrono/generator.h"
    "${CPP_HEADERS_DIR}/ma/chrono/generator_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/historylist.h"
    "${CPP_HEADERS_DIR}/ma/chrono/icdoctablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/icttablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/insurancetablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/lcredittablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/listpage_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/listtaskwindow_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/localeutility.h"
    "${CPP_HEADERS_DIR}/ma/chrono/modelutility.h"
    "${CPP_HEADERS_DIR}/ma/chrono/navigationbutton_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/navigationmodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/onefieldbasedfiltercondition.h"
    "${CPP_HEADERS_DIR}/ma/chrono/orderexpression.h"
    "${CPP_HEADERS_DIR}/ma/chrono/page_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/paysheettablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/residenttablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/resourcemanager.h"
    "${CPP_HEADERS_DIR}/ma/chrono/resourcemanager_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/sboltablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/sortdialog_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/sqlutility.h"
    "${CPP_HEADERS_DIR}/ma/chrono/tabledescription.h"
    "${CPP_HEADERS_DIR}/ma/chrono/taskwindowmanager_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/taskwindow_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/ttermtablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/types.h"
    "${CPP_HEADERS_DIR}/ma/chrono/types_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/unaryfiltercondition.h"
    "${CPP_HEADERS_DIR}/ma/chrono/userdefinedfilter.h"
    "${CPP_HEADERS_DIR}/ma/chrono/userdefinedorder.h"
    "${CPP_HEADERS_DIR}/ma/chrono/usertablemodel_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/widgetutility.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/autoenterprise.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/autoenterprise_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/bank.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/bank_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/bol.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/bol_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/ccd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/ccd_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/contract.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/contract_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/counterpart.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/counterpart_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/country.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/country_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/currency.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/currencyamount.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/currencyamount_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/currency_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/decimalamount.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/deppoint.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/deppoint_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/icdoc.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/icdoc_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/ict.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/ict_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/insurance.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/insurance_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/lcredit.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/lcredit_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/paysheet.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/paysheet_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/rateamount.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/rateamount_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/resident.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/resident_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/sbol.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/sbol_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/tterm.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/tterm_fwd.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/user.h"
    "${CPP_HEADERS_DIR}/ma/chrono/model/user_fwd.h"
    ${MOC_HEADERS})

set(MP_COMPAT_CPP_SOURCES
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
set(CPP_SOURCES 
    "${MP_COMPAT_CPP_SOURCES}"
    "${SOURCES_DIR}/ma/chrono/excelexporter.cpp")

set(NATIVE_RESOURCES )
set(NATIVE_RESOURCE_HEADERS )
if(WIN32)
    list(APPEND NATIVE_RESOURCES "${RESOURCES_DIR}/chrono.rc")
    list(APPEND NATIVE_RESOURCE_HEADERS "${RESOURCES_DIR}/resource.h")
endif()

set(LIBRARIES 
    "${Boost_LIBRARIES}"
    "${Qt_LIBRARIES}")

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
        list(APPEND LIBRARIES 
            "${Qt5Gui_EGL_LIBRARIES}"
            "${Qt5Gui_OPENGL_LIBRARIES}")

        # Add required platform integration plugins
        if(WIN32)
            list(APPEND LIBRARIES "Qt5::QWindowsIntegrationPlugin")
        else()
            # todo:
            # add support for the rest of platforms supported by Qt
            # by linking with platform specific plugin
            # (add platfrom integration plugin to LIBRARIES)
        endif()

        # Add Qt SVG and Qt SQL plugins
        list(APPEND LIBRARIES "Qt5::QSvgPlugin;Qt5::QSvgIconPlugin;Qt5::QIBaseDriverPlugin")

        # Some Qt libraries have no cmake files so wee need to search them ourselves
        # Refer to custom CMake modules:
        #     FindICU.cmake
        #     FindQt5PlatformSupport.cmake
        #     FindQt5HarfBuzzNG.cmake
        #     FindQt5FreeType.cmake
        #     FindQt5PCRE.cmake

        find_package(Qt5PlatformSupport REQUIRED)
        list(APPEND LIBRARIES "Qt5::PlatformSupport")

        find_package(ICU REQUIRED COMPONENTS i18n uc data)
        list(APPEND LIBRARIES "${ICU_LIBRARIES}")

        # Starting from Qt 5.4.0 HarfBuzz-NG library is included (at least Windows version of Qt 5.4.0)
        if(WIN32 AND NOT(${Qt5Gui_VERSION} VERSION_LESS "5.4.0"))
            find_package(Qt5HarfBuzzNG QUIET)
            if(Qt5HarfBuzzNG_FOUND)
                list(APPEND LIBRARIES "Qt5::HarfBuzzNG")
            endif()
        endif()

        # Starting from Qt 5.5.0 FreeType library is used by QtGui
        if(WIN32 AND NOT(${Qt5Gui_VERSION} VERSION_LESS "5.5.0"))
            find_package(Qt5FreeType QUIET)
            if(Qt5FreeType_FOUND)
                list(APPEND LIBRARIES "Qt5::FreeType")
            endif()
        endif()

        # Starting from Qt 5.5.0 PCRE library is used by QtCore
        if(WIN32 AND NOT(${Qt5Core_VERSION} VERSION_LESS "5.5.0"))
            find_package(Qt5PCRE QUIET)
            if(Qt5PCRE_FOUND)
                list(APPEND LIBRARIES "Qt5::PCRE")
            endif()
        endif()

        # Firebird client librray
        # Refer to custom CMake modules: 
        #     FindFirebirdClient.cmake

        find_package(FirebirdClient REQUIRED)
        list(APPEND LIBRARIES "Firebird::Client")
    endif()
else()
    # Statically linked Qt requires:
    #     All libraries used by Qt internally to be linked explicitly to the target.
    #     Used Qt plugins to be linked explicitly to the target.
    if(QT_IS_STATIC)
        # Add Qt SVG and Qt SQL plugins
        list(APPEND LIBRARIES "Qt4::qsvg;Qt4::qsvgicon;Qt4::qsqlibase")

        # Firebird client librray
        # Refer to custom CMake modules:
        #     FindFirebirdClient.cmake

        find_package(FirebirdClient REQUIRED)
        list(APPEND LIBRARIES "Firebird::Client")
    endif()
endif()

# Required Windows libraries
if(WIN32)
    list(APPEND LIBRARIES 
        "comdlg32"
        "uuid"
        "ole32"
        "oleaut32"
        "advapi32"
        "imm32"
        "winmm"
        "winspool"
        "ws2_32.lib")
endif()

# Boost chrono requires rt library on *nix
if(UNIX AND BOOST_HAS_CHRONO)
    list(APPEND LIBRARIES 
        "rt")
endif()

set(QM_FILES )

# Turn on parallel compilation of source files for MSVC.
# This otion speeds up overall build time
if(MSVC)
    if(NOT (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "16"))
        set_source_files_properties(
            "${MP_COMPAT_CPP_SOURCES}"
            "${MOC_GENERATED_SOURCES}"
            "${RCC_GENERATED_SOURCES}"
            PROPERTIES COMPILE_FLAGS "/MP")
    endif()
endif()

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CPP_HEADERS_DIR})

# Wrap Qt generated sources.
if(CHRONO_USE_QT5)
    qt5_wrap_cpp(MOC_GENERATED_SOURCES ${MOC_HEADERS})
    qt5_wrap_ui(UIC_GENERATED_HEADERS ${UIC_FORMS})
    qt5_add_resources(RCC_GENERATED_SOURCES ${QRC_RESOURCES})
    if(Qt5LinguistTools_FOUND)
        qt5_add_translation(QM_FILES ${TRANSLATION_SOURCES})
    endif()
else()
    qt4_wrap_cpp(MOC_GENERATED_SOURCES ${MOC_HEADERS})
    qt4_wrap_ui(UIC_GENERATED_HEADERS ${UIC_FORMS})
    qt4_add_resources(RCC_GENERATED_SOURCES ${QRC_RESOURCES})
    qt4_add_translation(QM_FILES ${TRANSLATION_SOURCES})
endif()

# Required for Qt UIC
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(chrono WIN32 
    ${CPP_HEADERS} ${CPP_SOURCES}
    ${NATIVE_RESOURCES} ${NATIVE_RESOURCE_HEADERS}
    ${MOC_GENERATED_SOURCES} ${RCC_GENERATED_SOURCES} ${UIC_GENERATED_HEADERS} ${QM_FILES}
    ${TRANSLATION_SOURCES})
target_link_libraries(chrono ${LIBRARIES})

source_group("Form Files" FILES
    ${UIC_FORMS})
source_group("Generated Files" FILES
    ${MOC_GENERATED_SOURCES} 
    ${QRC_GENERATED_SOURCES}
    ${UIC_GENERATED_HEADERS}
    ${QM_FILES})
source_group("Resource Files" FILES
    ${NATIVE_RESOURCES} 
    ${NATIVE_RESOURCE_HEADERS}
    ${QRC_RESOURCES})
source_group("Translation Files" FILES
    ${TRANSLATION_SOURCES})

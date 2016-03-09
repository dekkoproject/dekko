set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch -std=c++11")
set(CMAKE_AUTOMOC ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)
add_definitions(
  -DGETTEXT_PACKAGE=\"account-plugins\"
  -DGETTEXT_LOCALEDIR=\"${CMAKE_INSTALL_LOCALEDIR}\"
)
include(FindPkgConfig)
pkg_search_module(CONNECTIVITY REQUIRED connectivity-qt1)
if(NOT CONNECTIVITY_FOUND AND CONNECTIVITY_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find connectivity api, have you got libconnectivity-qt1-dev installed??")
endif()

include_directories(${CONNECTIVITY_INCLUDE_DIRS})
link_directories(${CONNECTIVITY_INCLUDE_DIRS})

#pkg_search_module(ACCOUNTS_QT REQUIRED accounts-qt5)
#if(NOT ACCOUNTS_QT_FOUND)
#    MESSAGE(FATAL_ERROR "Could no find accounts-qt5")
#else()
#    MESSAGE(STATUS "accounts-qt5 found: include dirs = ${ACCOUNTS_QT_INCLUDE_DIRS}")
#endif()
#include_directories(${ACCOUNTS_QT_INCLUDE_DIRS})
#link_directories(${ACCOUNTS_QT_INCLUDE_DIRS})

#pkg_search_module(SIGNON_QT REQUIRED libsignon-qt5)
#if(NOT SIGNON_QT_FOUND)
#    MESSAGE(FATAL_ERROR "Could not find signon-qt5, have you got libaccounts-qt5-dev installed??")
#else()
#    MESSAGE(STATUS "signon-qt5 found: include dirs = ${SIGNON_QT_INCLUDE_DIRS}")
#endif()
#include_directories(${SIGNON_QT_INCLUDE_DIRS})
#link_directories(${SIGNON_QT_INCLUDE_DIRS})

pkg_search_module(SASL2 REQUIRED libsasl2)
if(NOT SASL2_FOUND)
    MESSAGE(FATAL_ERROR "Could not find SASL2, have you got SASL2 installed??")
else()
    MESSAGE(STATUS "SASL2 found: include dirs = ${SASL2_INCLUDE_DIRS}")
endif()
include_directories(${SASL2_INCLUDE_DIRS})
link_directories(${SASL2_INCLUDE_DIRS})

set(EmailPlugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/emailplugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/trgettext.h

    ${CMAKE_CURRENT_SOURCE_DIR}/MailAutoDiscover/autodiscover.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MailAutoDiscover/autoconfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MailAutoDiscover/serverconfiguration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MailAutoDiscover/emailvalidator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MailAutoDiscover/srvlookup.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/MailServiceValidator/mailservicetask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MailServiceValidator/imaptask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MailServiceValidator/smtptask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MailServiceValidator/servicevalidator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MailServiceValidator/taskfactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/MailServiceValidator/saslmechanisms.cpp
)
add_library(EmailPluginLib MODULE
    ${EmailPlugin_SRCS}
)
set_target_properties(EmailPluginLib PROPERTIES
         LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/accounts/plugin/Email)

find_package(Qt5Core REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Xml REQUIRED)

qt5_use_modules(EmailPluginLib Core Gui Qml Quick Xml Network)
target_link_libraries(EmailPluginLib accounts-qt5 signon-qt5 sasl2 connectivity-qt1)

add_custom_target(EmailPluginLib-qmldir ALL
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/qmldir ${CMAKE_CURRENT_BINARY_DIR}/accounts/plugin/Email
    DEPENDS ${PLUGIN_QML_JS_FILES}
)

execute_process(
    COMMAND qmake -query QT_INSTALL_QML
        OUTPUT_VARIABLE QT_IMPORTS_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
install(TARGETS EmailPluginLib DESTINATION ${QT_IMPORTS_DIR}/accounts/plugin/Email)
install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/accounts/plugin/Email/qmldir DESTINATION ${QT_IMPORTS_DIR}/accounts/plugin/Email)

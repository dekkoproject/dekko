include(FindPkgConfig)

add_definitions(-DQMF_NO_MESSAGE_SERVICE_EDITOR -DPLUGIN_INTERNAL)
if(ENABLE_LOGGING)
    add_definitions(-DQMF_ENABLE_LOGGING)
endif()
if(USE_ONLINE_ACCOUNTS)
    add_definitions(-DUSE_ACCOUNTS_QT)
endif()

find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Xml 5.4 REQUIRED)
find_package(Qt5Network 5.4 REQUIRED)

if(USE_ONLINE_ACCOUNTS)
pkg_search_module(ACCOUNTS_QT REQUIRED accounts-qt5)
if(NOT ACCOUNTS_QT_FOUND)
    MESSAGE(FATAL_ERROR "Could no find accounts-qt")
else()
    MESSAGE(STATUS "accounts-qt found: include dirs = ${ACCOUNTS_QT_INCLUDE_DIRS}")
endif()
INCLUDE_DIRECTORIES(${ACCOUNTS_QT_INCLUDE_DIRS})
LINK_DIRECTORIES(${ACCOUNTS_QT_INCLUDE_DIRS})
pkg_search_module(SIGNON_QT REQUIRED libsignon-qt5)
if(NOT SIGNON_QT_FOUND)
    MESSAGE(FATAL_ERROR "Could not find signon-qt5, have you got libaccounts-qt5-dev installed??")
else()
    MESSAGE(STATUS "signon-qt5 found: include dirs = ${SIGNON_QT_INCLUDE_DIRS}")
endif()
include_directories(${SIGNON_QT_INCLUDE_DIRS})
link_directories(${SIGNON_QT_INCLUDE_DIRS})
endif()

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient/support
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfmessageserver
)

set(libSMTP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/smtpauthenticator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/smtpclient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/smtpconfiguration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/smtpservice.cpp
)

add_library(smtp SHARED ${libSMTP_SOURCES})
qt5_use_modules(smtp Core Network Xml)
target_link_libraries(smtp qmfclient5 qmfmessageserver5 accounts-qt5 signon-qt5)

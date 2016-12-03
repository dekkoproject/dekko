add_definitions(-DQT_QMF_HAVE_ZLIB -DQMF_NO_MESSAGE_SERVICE_EDITOR)
if(ENABLE_LOGGING)
    add_definitions(-DQMF_ENABLE_LOGGING)
endif()
if(USE_ONLINE_ACCOUNTS)
    add_definitions(-DUOA_ENABLED)
endif()

find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Network 5.4 REQUIRED)
find_package(Qt5Sql 5.4 REQUIRED)

if(ENABLE_UBUNTU_CONNECTIVITY)
    add_definitions(-DUSE_CONNECTIVITY_API)
    include(FindPkgConfig)
    pkg_search_module(CONNECTIVITY REQUIRED connectivity-qt1)
    if(NOT CONNECTIVITY_FOUND AND CONNECTIVITY_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find connectivity api, have you got libconnectivity-qt1-dev installed??")
    endif()

    link_directories(${CONNECTIVITY_INCLUDE_DIRS})
endif()

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QMFCLIENT_DIR}
    ${QMFCLIENT_DIR}/support
    ${QMFSERVER_DIR}
    ${CONNECTIVITY_INCLUDE_DIRS}
)

if(ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIR})
else()
    message(FATAL_ERROR " zlib required for IMAP COMPRESS. Check you have 'zlib' installed")
endif()


set(libIMAP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/imapclient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imapconfiguration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imapprotocol.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imapservice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imapstructure.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imapauthenticator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imapstrategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/integerregion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imaptransport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serviceactionqueue.cpp
)

add_library(imap SHARED ${libIMAP_SOURCES})
qt5_use_modules(imap Core Network DBus)
if(ENABLE_UBUNTU_CONNECTIVITY)
    target_link_libraries(imap ${ZLIB_LIBRARIES} qmfmessageserver5 qmfclient5 connectivity-qt1)
else()
    target_link_libraries(imap ${ZLIB_LIBRARIES} qmfmessageserver5 qmfclient5)
endif()

install(TARGETS imap DESTINATION ${MESSAGESERVICEPLUGINS_DIR})


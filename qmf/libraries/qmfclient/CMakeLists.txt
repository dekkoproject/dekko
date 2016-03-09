include(FindPkgConfig)

add_definitions(-DQT_BUILD_QCOP_LIB)
add_definitions(-DQMF_INTERNAL)
add_definitions(-DQMF_INSTALL_ROOT="${QMF_INSTALL_ROOT}")
if(ENABLE_LOGGING)
    add_definitions(-DQMF_ENABLE_LOGGING)
endif()

find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network 5.4 REQUIRED)
find_package(Qt5Sql 5.4 REQUIRED)
set(QMFCLIENT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QMFCLIENT_INCLUDE_DIR}
    ${QMFCLIENT_INCLUDE_DIR}/support
)

set(libQmfClient_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/longstream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/longstring.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailaccount.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailaccountconfiguration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailaccountkey.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailaccountlistmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailaccountsortkey.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailaction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailaddress.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailcodec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailcontentmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmaildatacomparator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmaildisconnected.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailfolder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailfolderfwd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailfolderkey.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailfoldersortkey.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailid.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailinstantiations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailkeyargument.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailmessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailmessagefwd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailmessagekey.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailmessagelistmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailmessagemodelbase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailmessageremovalrecord.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailmessageserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailmessageset.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailmessagesortkey.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailmessagethreadedmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailserviceaction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailstore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailstore_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailstoreimplementation_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailtimestamp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailthreadkey.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailthreadlistmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailthreadsortkey.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qprivateimplementation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/qmailnamespace.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/qmaillog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/qlogsystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/qloggers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/qcopadaptor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/qcopapplicationchannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/qcopchannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/qcopchannelmonitor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/qcopserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/qmailpluginmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/locks.cpp
)

set(RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/qmf.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qmf_icons.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qmf_qt.qrc
)

if(USE_ONLINE_ACCOUNTS)

set(SSO_OA_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ssoaccountmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ssosessionmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ssoauthplugin.cpp
)

endif()

qt5_add_resources(QMFCLIENT_RESOURCES ${RESOURCES})
if(USE_ONLINE_ACCOUNTS)
    message(STATUS "Linking accounts-qt & signon-qt")
    add_library(qmfclient5 SHARED ${libQmfClient_SOURCES} ${SSO_OA_SOURCES} ${QMFCLIENT_RESOURCES})
    target_link_libraries(qmfclient5 accounts-qt5 signon-qt5)
else()
    add_library(qmfclient5 SHARED ${libQmfClient_SOURCES} ${QMFCLIENT_RESOURCES})
endif()
qt5_use_modules(qmfclient5 Core Sql Network Gui)

install(TARGETS qmfclient5 DESTINATION ${QT_IMPORTS_DIR})

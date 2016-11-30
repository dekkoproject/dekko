if(UNITY8)
    add_definitions(-DQMF_NO_MESSAGE_SERVICE_EDITOR)
    add_definitions(-DIS_UNITY8)
else()
    add_definitions(-DQMF_NO_MESSAGE_SERVICE_EDITOR -DUSE_HTML_PARSER)
endif()
if(ENABLE_LOGGING)
    add_definitions(-DQMF_ENABLE_LOGGING)
endif()
if(USE_ONLINE_ACCOUNTS)
    add_definitions(-DUSE_ACCOUNTS_QT)
endif()

if(SNAP_MODE)
    add_definitions(-DSNAP)
endif(SNAP_MODE)

find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Gui 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QMFCLIENT_INCLUDES}
    ${QMFCLIENT_SUPPORT_INCLUDES}
    ${QMFSERVER_INCLUDES}
)
message(STATUS "CMAKE SOURCE DIR: ${CMAKE_SOURCE_DIR}")

if(SERVER_AS_THREAD)
    add_definitions(-DSERVER_AS_QTHREAD)
    set(MSGSERVER_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/mailmessageclient.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/messageserver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/prepareaccounts.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/newcountnotifier.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/servicehandler.cpp
    )
    add_library(msgserver SHARED ${MSGSERVER_SOURCES})
    qt5_use_modules(msgserver Core Gui)
    target_link_libraries(msgserver qmfmessageserver5 qmfclient5)
    install(TARGETS msgserver DESTINATION ${QT_IMPORTS_DIR})
else()
    set(MSGSERVER_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/mailmessageclient.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/messageserver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/prepareaccounts.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/newcountnotifier.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/servicehandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    )

    add_executable(messageserver5 ${MSGSERVER_SOURCES})
    qt5_use_modules(messageserver5 Core Gui)
    target_link_libraries(messageserver5 qmfmessageserver5 qmfclient5 stdc++)

    install(TARGETS messageserver5 DESTINATION ${BIN_DIR})
endif()

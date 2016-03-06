add_definitions(-DQMF_NO_MESSAGE_SERVICE_EDITOR -DUSE_HTML_PARSER)
if(ENABLE_LOGGING)
    add_definitions(-DQMF_ENABLE_LOGGING)
endif()
if(USE_ONLINE_ACCOUNTS)
    add_definitions(-DUSE_ACCOUNTS_QT)
endif()

find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Gui 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient/support
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfmessageserver
)
message(STATUS "CMAKE SOURCE DIR: ${CMAKE_SOURCE_DIR}")

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
target_link_libraries(messageserver5 qmfmessageserver5 qmfclient5)

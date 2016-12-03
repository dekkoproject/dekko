add_definitions(-DQMF_NO_MESSAGE_SERVICE_EDITOR -DMESSAGESERVER_INTERNAL)
if(ENABLE_LOGGING)
    add_definitions(-DQMF_ENABLE_LOGGING)
endif()

find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Network 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../qmfclient
    ${CMAKE_CURRENT_SOURCE_DIR}/../qmfclient/support
)

set(msgServer_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailauthenticator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailmessagebuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailmessageclassifier.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailmessageservice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailserviceconfiguration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailstoreaccountfilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailtransport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailheartbeattimer_qtimer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailheartbeattimer.h # messageserver5 won't link without this.
    ${CMAKE_CURRENT_SOURCE_DIR}/qmailmessageserverplugin.cpp
)

add_library(qmfmessageserver5 SHARED ${msgServer_SOURCES})
target_link_libraries(qmfmessageserver5 qmfclient5)
qt5_use_modules(qmfmessageserver5 Core Network)

install(TARGETS qmfmessageserver5 DESTINATION ${QMF_LIB_DIR})

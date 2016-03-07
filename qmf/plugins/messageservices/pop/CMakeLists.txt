add_definitions(-DQMF_NO_MESSAGE_SERVICE_EDITOR)
if(ENABLE_LOGGING)
    add_definitions(-DQMF_ENABLE_LOGGING)
endif()
if(USE_ONLINE_ACCOUNTS)
    add_definitions(-DUSE_ACCOUNTS_QT)
endif()

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient/support
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfmessageserver
)

set(libPOP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/popclient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/popconfiguration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/popservice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/popauthenticator.cpp
)

add_library(pop SHARED ${libPOP_SOURCES})
qt5_use_modules(pop Core Network)
target_link_libraries(pop qmfmessageserver5 qmfclient5)

install(TARGETS pop DESTINATION ${MESSAGESERVICEPLUGINS_DIR})

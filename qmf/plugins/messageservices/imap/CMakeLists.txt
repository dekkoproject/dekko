add_definitions(-DQT_QMF_HAVE_ZLIB -DQMF_NO_MESSAGE_SERVICE_EDITOR)
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

include_directories(${ZLIB_INCLUDE_DIR})

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
qt5_use_modules(imap Core Network)
target_link_libraries(imap ${ZLIB_LIBRARIES} qmfmessageserver5 qmfclient5)


find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)
find_package(Qt5Network 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient/support
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfmessageserver
    ${CMAKE_SOURCE_DIR}/plugins/shared
    ${CMAKE_SOURCE_DIR}/lib/mail
)

set(libNetwork_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/MsgPartQNAMFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MsgPartQNAM.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MsgPartReply.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MsgReply.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ForbiddenReply.cpp
)

add_library(network SHARED ${libNetwork_SRCS})

qt5_use_modules(network Core Quick Network)
target_link_libraries(network qmfclient5 qmfmessageserver5 mail)

install(TARGETS network DESTINATION ${QT_IMPORTS_DIR})

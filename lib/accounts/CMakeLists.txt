find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient/support
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfmessageserver
    ${CMAKE_SOURCE_DIR}/plugins/shared
)

set(libAccount_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/Account.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AccountConfiguration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Accounts.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AccountValidator.cpp
)

add_library(accounts SHARED ${libAccount_SRCS})
qt5_use_modules(accounts Core Quick)
target_link_libraries(accounts qmfclient5 qmfmessageserver5 dekkohelpers)

install(TARGETS accounts DESTINATION ${QT_IMPORTS_DIR})

if(USE_ONLINE_ACCOUNTS)
    add_subdirectory(oa-plugin)
endif()

find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient/support
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfmessageserver
)

set(AccountsPlugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/AccountsPlugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Account.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AccountConfiguration.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/AccountsListModel.cpp
)

add_library(accounts MODULE ${AccountsPlugin_SRCS})

qt5_use_modules(accounts Core Quick)
target_link_libraries(accounts qmfclient5 qmfmessageserver5)

set(ACCOUNTS_PLUGIN_DIR ${QT_IMPORTS_DIR}/Dekko/Accounts)

install(TARGETS accounts DESTINATION ${ACCOUNTS_PLUGIN_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qmldir DESTINATION ${ACCOUNTS_PLUGIN_DIR})

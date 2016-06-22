if(USE_ONLINE_ACCOUNTS)
    add_subdirectory(oa-plugin)
endif()

find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/lib/accounts
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient/support
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfmessageserver
    ${CMAKE_SOURCE_DIR}/plugins/shared
)

set(AccountsPlugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/AccountsPlugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PresetProviderModel.cpp
)

# We enable imap/pop3 support at the Accounts level by preventing creation of that type of account
if (ENABLE_IMAP_SUPPORT)
    add_definitions(-DIMAP_ENABLED)
endif(ENABLE_IMAP_SUPPORT)
if (ENABLE_POP3_SUPPORT)
    add_definitions(-DPOP3_ENABLED)
endif(ENABLE_POP3_SUPPORT)

add_library(accountsplugin MODULE ${AccountsPlugin_SRCS})

qt5_use_modules(accountsplugin Core Quick)
target_link_libraries(accountsplugin accounts dekkohelpers)

set(ACCOUNTS_PLUGIN_DIR ${QT_IMPORTS_DIR}/Dekko/Accounts)


install(TARGETS accountsplugin DESTINATION ${ACCOUNTS_PLUGIN_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qmldir DESTINATION ${ACCOUNTS_PLUGIN_DIR})

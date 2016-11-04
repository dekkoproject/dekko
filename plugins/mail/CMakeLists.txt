find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${ACCOUNTS_DIR}
    ${MAIL_DIR}
    ${QMFCLIENT_INCLUDES}
    ${QMFCLIENT_SUPPORT_INCLUDES}
    ${QMFSERVER_INCLUDES}
    ${SHARED_SRCS}
    ${SUPER_MACRO_INCLUDES}
)

set(MailPlugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/MailPlugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qmlenums.cpp
)

add_library(mailplugin MODULE ${MailPlugin_SRCS})

qt5_use_modules(mailplugin Core Quick)
target_link_libraries(mailplugin qmfclient5 qmfmessageserver5 dekkohelpers mail accounts)

set(MAIL_PLUGIN_DIR ${QT_IMPORTS_DIR}/Dekko/Mail)

install(TARGETS mailplugin DESTINATION ${MAIL_PLUGIN_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qmldir DESTINATION ${MAIL_PLUGIN_DIR})

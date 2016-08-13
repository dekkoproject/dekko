find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient/support
    ${CMAKE_SOURCE_DIR}/plugins/shared
    ${CMAKE_SOURCE_DIR}/lib/settings
)

set(SettingsPlugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsPlugin.cpp
)

add_library(settingsplugin MODULE ${SettingsPlugin_SRCS})

qt5_use_modules(settingsplugin Core Quick)
target_link_libraries(settingsplugin settings qmfclient5)

set(SETTINGS_PLUGIN_DIR ${QT_IMPORTS_DIR}/Dekko/Settings)

install(TARGETS settingsplugin DESTINATION ${SETTINGS_PLUGIN_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qmldir DESTINATION ${SETTINGS_PLUGIN_DIR})

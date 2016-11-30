find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QMFCLIENT_INCLUDES}
    ${QMFCLIENT_SUPPORT_INCLUDES}
    ${QMFSERVER_INCLUDES}
    ${SHARED_SRCS}
    ${SUPER_MACRO_INCLUDES}
)

set(libSettings_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsFileBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsObjectBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsPolicies.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PolicyManager.cpp
)

add_library(settings SHARED ${libSettings_SRCS})

qt5_use_modules(settings Core Quick)
target_link_libraries(settings qmfclient5 dekkohelpers)

install(TARGETS settings DESTINATION ${LIB_DIR})

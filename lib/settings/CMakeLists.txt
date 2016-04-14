find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/plugins/shared
)

set(libSettings_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsFileBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsObjectBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NavigationSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GlobalSettings.cpp
)

add_library(settings SHARED ${libSettings_SRCS})

qt5_use_modules(settings Core Quick)
target_link_libraries(settings dekkohelpers)

install(TARGETS settings DESTINATION ${QT_IMPORTS_DIR})

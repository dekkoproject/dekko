find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)
find_package(Qt5Svg 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SHARED_SRCS}
    ${SUPER_MACROS}
)

set(AccountsPlugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/ComponentsPlugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ImageHelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StretchColumn.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StretchRow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Stretcher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/LogRecorder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Introspector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PixelPerfectItem.cpp
)

add_library(componentsplugin MODULE ${AccountsPlugin_SRCS})

qt5_use_modules(componentsplugin Core Quick Svg)
target_link_libraries(componentsplugin dekkohelpers)

set(COMPONENTS_PLUGIN_DIR ${QT_IMPORTS_DIR}/Dekko/Components)


install(TARGETS componentsplugin DESTINATION ${COMPONENTS_PLUGIN_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qmldir DESTINATION ${COMPONENTS_PLUGIN_DIR})

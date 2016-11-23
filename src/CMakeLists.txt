find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${SHARED_SRCS}
)

set(libPlugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/PluginRegistry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PluginInfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PluginLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ActionRegistry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ServiceRegistry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ItemRegistry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ListenerRegistry.cpp
)


add_library(dekko-plugins SHARED ${libPlugin_SRCS})
qt5_use_modules(dekko-plugins Core Quick)

install(TARGETS dekko-plugins DESTINATION ${LIB_DIR})

set(PLUGIN_LIB_INC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(quick)

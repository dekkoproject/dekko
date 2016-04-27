find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/quickflux
    ${CMAKE_CURRENT_SOURCE_DIR}/quickflux/priv
    ${CMAKE_CURRENT_BINARY_DIR}/quickflux
    ${CMAKE_CURRENT_BINARY_DIR}/quickflux/priv
)

set(QUICKFLUX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/quickflux)
set(QuickFlux_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/quickflux_plugin.cpp
    ${QUICKFLUX_DIR}/qfapplistener.cpp
    ${QUICKFLUX_DIR}/qfappdispatcher.cpp
    ${QUICKFLUX_DIR}/qfappscript.cpp
    ${QUICKFLUX_DIR}/qfappscriptrunnable.cpp
    ${QUICKFLUX_DIR}/priv/qfappscriptrunnable.h
    ${QUICKFLUX_DIR}/qfappscriptdispatcherwrapper.cpp
    ${QUICKFLUX_DIR}/priv/qfappscriptdispatcherwrapper.h
    ${QUICKFLUX_DIR}/qflistener.cpp
    ${QUICKFLUX_DIR}/priv/qflistener.h
    ${QUICKFLUX_DIR}/qfapplistenergroup.cpp
    ${QUICKFLUX_DIR}/qfappscriptgroup.cpp
    ${QUICKFLUX_DIR}/qffilter.cpp
    ${QUICKFLUX_DIR}/qfkeytable.cpp
    ${QUICKFLUX_DIR}/priv/qfsignalproxy.cpp
    ${QUICKFLUX_DIR}/qfactioncreator.cpp
)

add_library(quickfluxplugin MODULE ${QuickFlux_SRCS})

qt5_use_modules(quickfluxplugin Core Quick)

set(QUICKFLUX_PLUGIN_DIR ${QT_IMPORTS_DIR}/QuickFlux)


install(TARGETS quickfluxplugin DESTINATION ${QUICKFLUX_PLUGIN_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qmldir DESTINATION ${QUICKFLUX_PLUGIN_DIR})

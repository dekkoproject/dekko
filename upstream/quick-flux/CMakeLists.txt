project(QuickFlux)

cmake_minimum_required(VERSION 3.0)
set(CMAKE_GENERATOR "Unix Makefiles")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC True)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "-O0 -g -Wall -Wsign-compare ${CMAKE_CXX_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "-O2 -Wall -Wsign-compare ${CMAKE_CXX_FLAGS} -s")
endif()

message(STATUS "CXXFLAGS ARE ${CMAKE_CXX_FLAGS}")

include(GNUInstallDirs)

if(DEFINED QT_IMPORTS_DIR)
    message(STATUS "Qt import dir already set, install QuickFlux to ${QT_IMPORTS_DIR}")
else()
    if(EXISTS "/etc/debian_version")
        set(QT_IMPORTS_DIR ${CMAKE_INSTALL_LIBDIR}/qt5/qml)
    else()
        set(QT_IMPORTS_DIR ${CMAKE_INSTALL_LIBDIR}/${CMAKE_LIBRARY_ARCHITECTURE}/qt5/qml)
    endif()
endif()

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

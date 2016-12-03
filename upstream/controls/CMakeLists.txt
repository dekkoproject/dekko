project(dekko-controls)
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
    message(STATUS "Qt import dir already set, install MazDB Quick Plugin to ${QT_IMPORTS_DIR}")
else()
    if(EXISTS "/etc/debian_version")
        set(QT_IMPORTS_DIR ${CMAKE_INSTALL_LIBDIR}/qt5/qml)
    else()
        set(QT_IMPORTS_DIR ${CMAKE_INSTALL_LIBDIR}/${CMAKE_LIBRARY_ARCHITECTURE}/qt5/qml)
    endif()
endif()

find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)
find_package(Qt5Svg 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(ControlsPlugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ControlsPlugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/QQuickStack.cpp
)

add_library(controlsplugin MODULE ${ControlsPlugin_SRCS})

qt5_use_modules(controlsplugin Core Quick Svg)

set(CONTROLS_PLUGIN_DIR ${QT_IMPORTS_DIR}/Dekko/Controls)

set(QML_CTRL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StackView.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StackViewTransition.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StackViewDelegate.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StackSlideDelegate.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Stack.js
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qmldir
)
add_custom_target(DekkoControlsQml ALL SOURCES ${QML_CTRL_SRCS})


install(TARGETS controlsplugin DESTINATION ${CONTROLS_PLUGIN_DIR})
install(FILES ${QML_CTRL_SRCS} DESTINATION ${CONTROLS_PLUGIN_DIR})


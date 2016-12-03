find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)
find_package(Qt5Svg 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(ControlsPlugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/ControlsPlugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/QQuickStack.cpp
)

add_library(controlsplugin MODULE ${ControlsPlugin_SRCS})

qt5_use_modules(controlsplugin Core Quick Svg)

set(CONTROLS_PLUGIN_DIR ${QT_IMPORTS_DIR}/Dekko/Controls)

set(QML_CTRL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/StackView.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/StackViewTransition.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/StackViewDelegate.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/StackSlideDelegate.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/Stack.js
    ${CMAKE_CURRENT_SOURCE_DIR}/qmldir
)
add_custom_target(DekkoControlsQml ALL SOURCES ${QML_CTRL_SRCS})


install(TARGETS controlsplugin DESTINATION ${CONTROLS_PLUGIN_DIR})
install(FILES ${QML_CTRL_SRCS} DESTINATION ${CONTROLS_PLUGIN_DIR})

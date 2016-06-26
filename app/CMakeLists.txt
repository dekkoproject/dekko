find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)
find_package(Qt5Qml 5.4 REQUIRED)
find_package(Qt5Gui 5.4 REQUIRED)
find_package(Qt5Widgets 5.4 REQUIRED)

file(GLOB_RECURSE DEKKO_QML_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    qml/*.qml *.js
    icons/*svg icons/*.png
)

set(DEKKO_QML_RESOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/dekkoqml.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/actions/dekkoactions.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/stores/dekkostores.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/constants/dekkocontants.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/views/components/dekkocomponents.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/views/delegates/dekkodelegates.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/views/dialogs/dekkodialogs.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/views/models/dekkomodels.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/views/popovers/dekkopopups.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/views/setupwizard/dekkowizard.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/views/stages/dekkostages.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/views/utils/dekkoutils.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/views/webview/dekkoweb.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/views/composer/dekkocomposer.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/views/toasts/dekkotoasts.qrc
)

set(DEKKO_ICON_RESOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/icons/actions/actionicons.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/icons/filetypes/filetypeicons.qrc
)

set(DEKKO_PROVIDERS_RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/icons/providers/providericons.qrc)

add_custom_target(DekkoQml ALL SOURCES ${DEKKO_QML_FILES})
add_custom_target(DekkoIcons ALL SOURCES ${DEKKO_ICON_RESOURCE})
add_custom_target(DekkoProviders ALL SOURCES ${DEKKO_PROVIDERS_RESOURCE})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient/support
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfmessageserver
    ${CMAKE_SOURCE_DIR}/lib/network
)

set(DEKKO_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Dekko.cpp
)

qt5_add_resources(DEKKO_RESOURCES
    ${DEKKO_QML_RESOURCE}
    ${DEKKO_ICON_RESOURCE}
    ${DEKKO_PROVIDERS_RESOURCE}
)

if(CLICK_MODE)
    add_definitions(-DCLICK_MODE)
endif()

if(ENABLE_QML_DEBUG)
    add_definitions(-DQT_QML_DEBUG)
endif()

add_executable(dekko ${DEKKO_SOURCES} ${DEKKO_RESOURCES})
qt5_use_modules(dekko Gui Core Quick Qml Widgets)
target_link_libraries(dekko qmfclient5 qmfmessageserver5 network)

install(TARGETS dekko DESTINATION ${BIN_DIR})
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/qml/views/webview/userscripts
    DESTINATION ${DATA_DIR}
)
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/configuration
    DESTINATION ${DATA_DIR}
)

#destop file configuration
set(ICON dekko.png)
set(ICON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/icons/dekko/dekko.png)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dekko.desktop ${CMAKE_CURRENT_BINARY_DIR}/dekko.desktop)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/dekko.desktop
    DESTINATION ${DATA_DIR}
)
install(FILES ${ICON_PATH} DESTINATION ${DATA_DIR})

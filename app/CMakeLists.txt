find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)
find_package(Qt5Qml 5.4 REQUIRED)
find_package(Qt5Gui 5.4 REQUIRED)

file(GLOB_RECURSE DEKKO_QML_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    qml/*.qml *.js
    icons/*svg
)

set(DEKKO_QML_RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/qml/dekkoqml.qrc)
set(DEKKO_ICON_RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/icons/actions/actionicons.qrc)
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

add_executable(dekko ${DEKKO_SOURCES} ${DEKKO_RESOURCES})
qt5_use_modules(dekko Gui Core Quick Qml)
target_link_libraries(dekko qmfclient5 qmfmessageserver5 network)

install(TARGETS dekko DESTINATION ${BIN_DIR})

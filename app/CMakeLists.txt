add_subdirectory(backend)

find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)
find_package(Qt5Qml 5.4 REQUIRED)
find_package(Qt5Gui 5.4 REQUIRED)

file(GLOB_RECURSE DEKKO_QML_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    qml/*.qml *.js
)

set(DEKKO_QML_RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/qml/dekkoqml.qrc)

add_custom_target(DekkoQml ALL SOURCES ${DEKKO_QML_FILES})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient/support
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfmessageserver
)

set(DEKKO_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Dekko.cpp
)

qt5_add_resources(DEKKO_RESOURCES ${DEKKO_QML_RESOURCE})

add_executable(dekko ${DEKKO_SOURCES} ${DEKKO_RESOURCES})
qt5_use_modules(dekko Gui Core Quick Qml)
target_link_libraries(dekko qmfclient5 qmfmessageserver5)

install(TARGETS dekko DESTINATION ${BIN_DIR})

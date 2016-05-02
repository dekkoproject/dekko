find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)
find_package(Qt5Xml 5.4 REQUIRED)

include(FindPkgConfig)
pkg_search_module(CONNECTIVITY REQUIRED connectivity-qt1)
if(NOT CONNECTIVITY_FOUND AND CONNECTIVITY_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find connectivity api, have you got libconnectivity-qt1-dev installed??")
endif()

link_directories(${CONNECTIVITY_INCLUDE_DIRS})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient/support
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfmessageserver
    ${CMAKE_SOURCE_DIR}/plugins/shared
    ${CONNECTIVITY_INCLUDE_DIRS}
    ${SUPER_MACRO_INCLUDES}
)

set(AutoConfigPlugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/emailprovider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autoconfigplugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autoconfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autodiscover.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/emailvalidator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serverconfiguration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/srvlookup.cpp
)

add_library(autoconfig MODULE ${AutoConfigPlugin_SRCS})

qt5_use_modules(autoconfig Core Quick Xml)
target_link_libraries(autoconfig qmfclient5 qmfmessageserver5 connectivity-qt1 dekkohelpers)

set(AUTOCONFIG_PLUGIN_DIR ${QT_IMPORTS_DIR}/Dekko/AutoConfig)

install(TARGETS autoconfig DESTINATION ${AUTOCONFIG_PLUGIN_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qmldir DESTINATION ${AUTOCONFIG_PLUGIN_DIR})

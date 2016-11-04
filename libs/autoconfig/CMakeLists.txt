find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)
find_package(Qt5Xml 5.4 REQUIRED)


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${SHARED_SRCS}
    ${SUPER_MACRO_INCLUDES}
)

set(USE_CONNECTIVITY False)
if(USE_UBUNTU_CONNECTIVITY)
    include(FindPkgConfig)
    pkg_search_module(CONNECTIVITY REQUIRED connectivity-qt1)
    if(NOT CONNECTIVITY_FOUND AND CONNECTIVITY_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find connectivity api, have you got libconnectivity-qt1-dev installed??")
    endif()

    link_directories(${CONNECTIVITY_INCLUDE_DIRS})

    include_directories(
        ${CONNECTIVITY_INCLUDE_DIRS}
    )
    add_definitions(-DUSE_CONNECTIVITY_API)
    set(USE_CONNECTIVITY True)
else()

endif()

set(AutoConfigPlugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/emailprovider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/autoconfigplugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/autoconfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/autodiscover.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/serverconfiguration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/srvlookup.cpp
)

add_library(autoconfig MODULE ${AutoConfigPlugin_SRCS})

qt5_use_modules(autoconfig Core Quick Xml)

if(USE_CONNECTIVITY)
    target_link_libraries(autoconfig connectivity-qt1 dekkohelpers)
else()
    target_link_libraries(autoconfig dekkohelpers)
endif()

set(AUTOCONFIG_PLUGIN_DIR ${QT_IMPORTS_DIR}/Dekko/AutoConfig)

install(TARGETS autoconfig DESTINATION ${AUTOCONFIG_PLUGIN_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/qmldir DESTINATION ${AUTOCONFIG_PLUGIN_DIR})

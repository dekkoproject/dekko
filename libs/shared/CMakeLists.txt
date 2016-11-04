find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)
find_package(Qt5Gui 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SharedLib_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/QmlObjectListModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/QmlVariantListModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Paths.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Formatting.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/emailvalidator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RowsJoinerProxy.cpp
)

add_library(dekkohelpers SHARED ${SharedLib_SRCS})
qt5_use_modules(dekkohelpers Core Gui Quick)

install(TARGETS dekkohelpers DESTINATION ${LIB_DIR})

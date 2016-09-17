find_package(Qt5Core 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/plugins/contacts
    ${CMAKE_SOURCE_DIR}/plugins/shared/src
)

set(libABookPlugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/AbookPlugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ABook.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ABookAddressBook.cpp
)

add_library(abook-plugin SHARED ${libABookPlugin_SRCS})
qt5_use_modules(abook-plugin Core)
target_link_libraries(abook-plugin dekkohelpers contacts)

install(TARGETS abook-plugin DESTINATION ${DEKKO_PLUGIN_DIR}/contacts)

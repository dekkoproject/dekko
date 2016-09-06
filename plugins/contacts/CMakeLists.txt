find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/plugins/shared
    ${SUPER_MACRO_INCLUDES}
)

set(libContactsPlugins_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/Contact.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ContactsPluginInterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ContactsPlugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ContactsPluginLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AddressBooks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AddressBook.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CombinedAddressBookModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SortedContactsModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SearchFilter.cpp
)

add_definitions(-DQTQMLTRICKS_NO_PREFIX_ON_GETTERS)
add_definitions(-DDEKKO_PLUGINS_DIR="/${DEKKO_PLUGIN_DIR}")

add_library(contacts SHARED ${libContactsPlugins_SRCS})
qt5_use_modules(contacts Core)
target_link_libraries(contacts dekkohelpers)

install(TARGETS contacts DESTINATION ${LIB_DIR})

# Quick plugin
set(libQuickContacts_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/QuickContactsPlugin.cpp
)

find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)

add_library(quick-contacts MODULE ${libQuickContacts_SRCS})
qt5_use_modules(quick-contacts Core Quick)
target_link_libraries(quick-contacts contacts)

set(QCONTACTS_PLUGIN_DIR ${QT_IMPORTS_DIR}/Dekko/Contacts)
install(TARGETS quick-contacts DESTINATION ${QCONTACTS_PLUGIN_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qmldir DESTINATION ${QCONTACTS_PLUGIN_DIR})

add_subdirectory(abook)
add_subdirectory(vdir)
add_subdirectory(ldap)
add_subdirectory(system)

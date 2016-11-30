find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${SHARED_SRCS}
    ${MAZDB_SRCS}
)

set(addressbook_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/AddressBooks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AddressBook.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Collection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Contact.cpp
)

add_definitions(-DQTQMLTRICKS_NO_PREFIX_ON_GETTERS)
#add_definitions(-DDEKKO_PLUGINS_DIR="/${DEKKO_PLUGIN_DIR}")

add_library(addressbook SHARED ${addressbook_SRCS})
qt5_use_modules(addressbook Core)
target_link_libraries(addressbook dekkohelpers mazdb)

install(TARGETS addressbook DESTINATION ${LIB_DIR})


set(ABOOK_PLUGIN
    ${CMAKE_CURRENT_SOURCE_DIR}/AddressBookPlugin.cpp
)
add_library(addressbook-plugin MODULE ${ABOOK_PLUGIN})
qt5_use_modules(addressbook-plugin Core Quick)
target_link_libraries(addressbook-plugin dekkohelpers mazdb addressbook)

set(ABOOK_PLUGIN_DIR ${QT_IMPORTS_DIR}/Dekko/Contacts)
install(TARGETS addressbook-plugin DESTINATION ${ABOOK_PLUGIN_DIR})

set(ABOOK_QML_AND_SRCS
   ${CMAKE_CURRENT_SOURCE_DIR}/qml/ContactActions.qml
   ${CMAKE_CURRENT_SOURCE_DIR}/qml/ContactKeys.qml
   ${CMAKE_CURRENT_SOURCE_DIR}/qml/ContactsStore.qml
   ${CMAKE_CURRENT_SOURCE_DIR}/qml/ContactsWorker.qml
   ${CMAKE_CURRENT_SOURCE_DIR}/qml/CollectionListModel.qml
   ${CMAKE_CURRENT_SOURCE_DIR}/qml/AddressBookRegistry.qml
   ${CMAKE_CURRENT_SOURCE_DIR}/qmldir
)
install(FILES ${ABOOK_QML_AND_SRCS} DESTINATION ${ABOOK_PLUGIN_DIR})

set(ABOOK_QML_PLUGIN
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/plugin/AddressBookPlugin.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/plugin/Event.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/plugin/qmldir
)
install(FILES ${ABOOK_QML_PLUGIN} DESTINATION ${ABOOK_PLUGIN_DIR}/Plugin)

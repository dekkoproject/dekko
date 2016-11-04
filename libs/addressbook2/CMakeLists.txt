
add_subdirectory(src)

if(ENABLE_ADDRESSBOOK_TESTS)
    enable_testing()
    add_subdirectory(libs/mace)
    set(MACE_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/libs/mace)
    add_subdirectory(tests)
endif()

file(GLOB_RECURSE ADDRESSBOOK_QML_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.qml *.yml qmldir
)
add_custom_target(AddressBookQml ALL SOURCES ${ADDRESSBOOK_QML_FILES})

if(DEFINED BIN_DIR)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/plugins/Default DESTINATION ${BIN_DIR}/plugins/contacts)
endif()

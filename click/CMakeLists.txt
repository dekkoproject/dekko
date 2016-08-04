file(GLOB_RECURSE CLICK_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} * *.*
)
add_custom_target(ClickSrcs ALL SOURCES ${CLICK_FILES})

if (UNCONFINED)
    set(APPARMOR_MANIFEST ${CMAKE_CURRENT_SOURCE_DIR}/openstore/dekko.apparmor)
else()
    set(APPARMOR_MANIFEST ${CMAKE_CURRENT_SOURCE_DIR}/dekko.apparmor)
endif()

set(CLICK_MANIFEST ${CMAKE_CURRENT_SOURCE_DIR}/dekko.json)

set(OTHER_MANIFEST
    ${CMAKE_CURRENT_SOURCE_DIR}/.excludes
    ${APPARMOR_MANIFEST}
    ${CMAKE_CURRENT_SOURCE_DIR}/dekko.dekkoproject_dekko.application
    ${CMAKE_CURRENT_SOURCE_DIR}/dekko.dekkoproject_dekko.service
    ${CMAKE_CURRENT_SOURCE_DIR}/dekko.url-dispatcher
    ${CMAKE_CURRENT_SOURCE_DIR}/dekko-content.json
    ${CMAKE_CURRENT_SOURCE_DIR}/dekkohelper-aa.json
    ${CMAKE_CURRENT_SOURCE_DIR}/dekko-helper.json
)

if (CLICK_MODE)
    configure_file(${CLICK_MANIFEST} ${CMAKE_CURRENT_BINARY_DIR}/manifest.json)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/manifest.json
        ${OTHER_MANIFEST}
        DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
    install(
        PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/dekko-launch
        DESTINATION ${BIN_DIR}
    )
    install(
        PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/dekko-helper
        DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)
# Make the unit test files visible on qtcreator
add_custom_target(DATA_FILES ALL SOURCES
    "e-mail.provider"
    "e-mail.service"
)
set(ACCOUNT_PROVIDER_DIR ${CMAKE_INSTALL_DATADIR}/accounts/providers)
install(FILES "e-mail.provider" DESTINATION ${ACCOUNT_PROVIDER_DIR})
set(ACCOUNT_SERVICE_DIR ${CMAKE_INSTALL_DATADIR}/accounts/services)
install(FILES "e-mail.service" DESTINATION ${ACCOUNT_SERVICE_DIR})

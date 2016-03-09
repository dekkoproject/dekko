file(GLOB PLUGIN_QML_JS_FILES *.qml *.js)

# Make the files visible on qtcreator
add_custom_target(plugin_QMlFiles ALL SOURCES ${PLUGIN_QML_JS_FILES})

set(ACCOUNT_QML_PLUGIN_DIR ${CMAKE_INSTALL_DATADIR}/accounts/qml-plugins/e-mail)

install(FILES ${PLUGIN_QML_JS_FILES} DESTINATION ${ACCOUNT_QML_PLUGIN_DIR})

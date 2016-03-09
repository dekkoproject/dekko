include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)
# Make the unit test files visible on qtcreator
add_custom_target(MailAutoDiscoverbackend_UNITTEST_QML_FILES ALL SOURCES
    "unit/tst_emailvalidator.qml"
    "unit/tst_autodiscover.qml"
    "unit/tst_serversettings.qml"
)

add_custom_target("check"
                    COMMAND /usr/bin/qmltestrunner -input ${CMAKE_SOURCE_DIR}/tests/unit -import ${CMAKE_BINARY_DIR}/src
                    DEPENDS EmailPluginLib EmailPluginLib-qmldir)

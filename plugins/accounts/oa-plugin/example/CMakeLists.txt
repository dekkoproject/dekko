file(GLOB QML_JS_FILES *.qml *.js)

# Make the files visible on qtcreator
add_custom_target(mailautodiscover_QMlFiles ALL SOURCES ${QML_JS_FILES})

add_custom_target("run" /usr/bin/qmlscene -I ${CMAKE_BINARY_DIR}/src ${CMAKE_SOURCE_DIR}/example/example.qml
                    DEPENDS EmailPluginLib EmailPluginLib-qmldir)

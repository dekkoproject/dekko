import qbs

DynamicLibrary {
    name: "AddressBook Plugin"

    Group {
        name: "Plugin Files"
        prefix: path + "/"
        files: [
            "*.dekko-plugin",
            "DefaultPlugin.qml"
        ]
        qbs.install: true
        qbs.installDir: project.pluginDir + "/contacts/default-abook"
    }
}

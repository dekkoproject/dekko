import qbs

Project {
    id: notify
    name: "Notify QML"

    property string qmlDir: "lib"
    property string pluginDir: "Dekko/Notify"

    DynamicLibrary {
        name: "Notify Plugin"

        Group {
            name: "QML Sources"
            files: ["src/**"]
            qbs.install: true
            qbs.installDir: notify.qmlDir + "/" + notify.pluginDir
            qbs.installSourceBase: "src"
        }
    }
}

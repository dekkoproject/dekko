import qbs

Project {
    name: "Ubuntu UI"

    Product {
        name: "Ubuntu UI Plugin"

        Depends { name: "cpp" }
        Depends { name: "Qt.core" }

        Group {
            name: "QML components"
            files: [
                "qml/**/*.qml"
            ]
            fileTags: ["ui-plugin"]
        }

        Group {
            name: "JavaScript modules"
            files: [
                "qml/**/*.js"
            ]
            fileTags: ["ui-plugin"]
        }

        Group {
            name: "QML directory"
            files: [
                "qml/constants/qmldir"
            ]
            fileTags: ["ui-plugin"]
        }

        Group {
            name: "Plugin specs"
            files: [
                "qml/**/*.dekko-plugin"
            ]
            fileTags: ["ui-plugin"]
        }

        Group {
            condition: project.ui === "ubuntu"
            fileTagsFilter: "ui-plugin"
            qbs.install: project.ui === "ubuntu"
            qbs.installDir: project.binDir + "/plugins/ui/"
            qbs.installSourceBase: "qml"
        }
    }
}

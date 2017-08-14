import qbs

Project {
    name: "Ubuntu UI"
    property bool enabled: false

    references: [
        "plugins/default-plugins.qbs",
        "src/stages/stages.qbs",
        "src/constants/constants.qbs",
        "src/components/components.qbs",
        "src/utils/helpers.qbs",
        "src/dialogs/dialogs.qbs"
    ]

    Product {
        name: "QML UI"
        condition: project.enabled

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

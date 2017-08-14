import qbs

Product {
    name: "Dialogs"
    condition: project.enabled

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }

    Group {
        name: "QML components"
        files: [
            "*.qml"
        ]
        fileTags: ["dialog-plugin"]
    }

    Group {
        name: "JavaScript modules"
        files: [
            "*.js"
        ]
        fileTags: ["dialog-plugin"]
    }

    Group {
        name: "QML directory"
        files: [
            "qmldir"
        ]
        fileTags: ["dialog-plugin"]
    }

    Group {
        condition: project.ui === "ubuntu"
        fileTagsFilter: "dialog-plugin"
        qbs.install: project.ui === "ubuntu"
        qbs.installDir: project.qmlDir + "/Dekko/Ubuntu/Dialogs"
    }
}

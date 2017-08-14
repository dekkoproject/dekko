import qbs

Product {
    name: "Utils"
    condition: project.enabled

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }

    Group {
        name: "QML components"
        files: [
            "*.qml"
        ]
        fileTags: ["utils-plugin"]
    }

    Group {
        name: "JavaScript modules"
        files: [
            "*.js"
        ]
        fileTags: ["utils-plugin"]
    }

    Group {
        name: "QML directory"
        files: [
            "qmldir"
        ]
        fileTags: ["utils-plugin"]
    }

    Group {
        condition: project.ui === "ubuntu"
        fileTagsFilter: "utils-plugin"
        qbs.install: project.ui === "ubuntu"
        qbs.installDir: project.qmlDir + "/Dekko/Ubuntu/Helpers"
    }
}

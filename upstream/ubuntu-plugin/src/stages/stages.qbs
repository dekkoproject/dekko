import qbs

Product {
    name: "Stages"
    condition: project.enabled

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }

    Group {
        name: "QML components"
        files: [
            "*.qml"
        ]
        fileTags: ["stages-plugin"]
    }

    Group {
        name: "JavaScript modules"
        files: [
            "*.js"
        ]
        fileTags: ["stages-plugin"]
    }

    Group {
        name: "QML directory"
        files: [
            "qmldir"
        ]
        fileTags: ["stages-plugin"]
    }

    Group {
        condition: project.ui === "ubuntu"
        fileTagsFilter: "stages-plugin"
        qbs.install: project.ui === "ubuntu"
        qbs.installDir: project.qmlDir + "/Dekko/Ubuntu/Stage"
    }
}

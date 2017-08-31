import qbs

Product {
    name: "Constants"
    condition: project.enabled

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }

    Group {
        name: "QML components"
        files: [
            "*.qml"
        ]
        fileTags: ["constants-plugin"]
    }

    Group {
        name: "JavaScript modules"
        files: [
            "*.js"
        ]
        fileTags: ["constants-plugin"]
    }

    Group {
        name: "QML directory"
        files: [
            "qmldir"
        ]
        fileTags: ["constants-plugin"]
    }

    Group {
        condition: project.ui === "ubuntu"
        fileTagsFilter: "constants-plugin"
        qbs.install: project.ui === "ubuntu"
        qbs.installDir: project.qmlDir + "/Dekko/Ubuntu/Constants"
    }
}

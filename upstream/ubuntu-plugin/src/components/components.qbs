import qbs

Project {
    name: "Components"

    Product {
        name: "UI Components"
        condition: project.enabled

        Depends { name: "cpp" }
        Depends { name: "Qt.core" }

        Group {
            name: "QML components"
            files: [
                "*.qml"
            ]
            fileTags: ["components-plugin"]
        }

        Group {
            name: "JavaScript modules"
            files: [
                "*.js"
            ]
            fileTags: ["components-plugin"]
        }

        Group {
            name: "QML directory"
            files: [
                "qmldir"
            ]
            fileTags: ["components-plugin"]
        }

        Group {
            condition: project.ui === "ubuntu"
            fileTagsFilter: "components-plugin"
            qbs.install: project.ui === "ubuntu"
            qbs.installDir: project.qmlDir + "/Dekko/Ubuntu/Components"
        }
    }

    Product {
        name: "Private Components"
        condition: project.enabled

        Depends { name: "cpp" }
        Depends { name: "Qt.core" }

        Group {
            name: "QML components"
            files: [
                "private/*.qml"
            ]
            fileTags: ["priv-components-plugin"]
        }

        Group {
            name: "JavaScript modules"
            files: [
                "private/*.js"
            ]
            fileTags: ["priv-components-plugin"]
        }

        Group {
            condition: project.ui === "ubuntu"
            fileTagsFilter: "priv-components-plugin"
            qbs.install: project.ui === "ubuntu"
            qbs.installDir: project.qmlDir + "/Dekko/Ubuntu/Components/private"
        }

    }

}

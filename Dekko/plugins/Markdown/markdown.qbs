import qbs

Project {
    name: "Markdown"

    Product {
        name: "Markdown Plugin"
        type: "markdown-plugin"

        Group {
            name: "Python Modules"
            files: "*.py"
            fileTags: ["md-plugin"]
        }

        Group {
            files: [
                "*.qml",
            ]
            name: "QML components"
            fileTags: ["md-plugin"]
        }

        Group {
            name: "QML directory"
            files: "qmldir"
            fileTags: ["md-plugin"]
        }

        Group {
            fileTagsFilter: "md-plugin"
            qbs.install: true
            qbs.installDir: project.qmlDir + "/Dekko/" + project.name
            qbs.installSourceBase: path
        }
    }
}

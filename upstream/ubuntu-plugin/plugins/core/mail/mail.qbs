import qbs

Project {
    name: "Mail"

    Product {
        name: "Mail UI"
        condition: project.enabled

        Depends { name: "cpp" }
        Depends { name: "Qt.core" }

        Group {
            name: "QML components"
            files: [
                "*.qml",
                "**/*.qml"
            ]
            fileTags: ["mail-ui-plugin"]
        }

        Group {
            name: "JavaScript modules"
            files: [
                "**/*.js"
            ]
            fileTags: ["mail-ui-plugin"]
        }

        Group {
            name: "Plugin specs"
            files: [
                "**/*.dekko-plugin"
            ]
            fileTags: ["mail-ui-plugin"]
        }

        Group {
            condition: project.ui === "ubuntu"
            fileTagsFilter: "mail-ui-plugin"
            qbs.install: project.ui === "ubuntu"
            qbs.installDir: project.binDir + "/plugins/ui/mail"
        }
    }
}

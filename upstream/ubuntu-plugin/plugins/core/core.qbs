import qbs

Project {
    name: "Core"

    Product {
        name: "Mail"
        condition: project.enabled

        Depends { name: "cpp" }
        Depends { name: "Qt.core" }

        Group {
            name: "QML components"
            files: [
                "mail/*.qml",
                "mail/**/*.qml"
            ]
            fileTags: ["mail-ui-plugin"]
        }

        Group {
            name: "JavaScript modules"
            files: [
                "mail/**/*.js"
            ]
            fileTags: ["mail-ui-plugin"]
        }

        Group {
            name: "Plugin specs"
            files: [
                "mail/**/*.dekko-plugin"
            ]
            fileTags: ["mail-ui-plugin"]
        }

        Group {
            condition: project.ui === "ubuntu"
            fileTagsFilter: "mail-ui-plugin"
            qbs.install: project.ui === "ubuntu"
            qbs.installDir: project.binDir + "/plugins/ubuntu/mail"
            qbs.installSourceBase: "mail"
        }
    }

    Product {
        name: "Settings"
        condition: project.enabled

        Depends { name: "cpp" }
        Depends { name: "Qt.core" }

        Group {
            name: "QML components"
            files: [
                "settings/*.qml",
                "settings/**/*.qml"
            ]
            fileTags: ["settings-ui-plugin"]
        }

        Group {
            name: "JavaScript modules"
            files: [
                "settings/**/*.js"
            ]
            fileTags: ["settings-ui-plugin"]
        }

        Group {
            name: "Plugin specs"
            files: [
                "settings/**/*.dekko-plugin"
            ]
            fileTags: ["settings-ui-plugin"]
        }

        Group {
            condition: project.ui === "ubuntu"
            fileTagsFilter: "settings-ui-plugin"
            qbs.install: project.ui === "ubuntu"
            qbs.installDir: project.binDir + "/plugins/ubuntu/settings"
            qbs.installSourceBase: "settings"
        }
    }

//    SubProject {
//        filePath: "mail/mail.qbs"
//        inheritProperties: true
//    }
}

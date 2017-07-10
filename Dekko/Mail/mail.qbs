import qbs

Project {
    name: "Mail"

    // API
    DynamicLibrary {
        name: "API"
        Group {
            name: "Public API"
            files: ["api/public/**"]
            fileTags: ["public-qml-api"]
            qbs.install: true
            qbs.installDir: project.qmlDir + "/Dekko/Mail/API"
            qbs.installSourceBase: "api/public"
        }

        readonly property stringList qmlImportPaths: [ qbs.installRoot + "/" + project.qmlDir ]


        Group {
            name: "Private API"
            files: ["api/private/**"]
            fileTags: ["private-qml-api"]
            qbs.install: true
            qbs.installDir: project.qmlDir + "/Dekko/Mail/API/Private"
            qbs.installSourceBase: "api/private"
        }
    }

    DynamicLibrary {
        name: "Stores"

        Group {
            name: "Mail Stores"
            files: ["stores/**"]
            fileTags: ["mail-stores"]
            qbs.install: true
            qbs.installDir: project.qmlDir + "/Dekko/Mail/Stores"
            qbs.installSourceBase: "stores"
        }
        readonly property stringList qmlImportPaths: [
            qbs.installRoot + "/" + project.qmlDir + "/"
        ]
    }

    DynamicLibrary {
        name: "Workers"

        Group {
            name: "Mail Workers"
            files: ["workers/**"]
            fileTags: ["mail-workers"]
            qbs.install: true
            qbs.installDir: project.qmlDir + "/Dekko/Mail/Workers"
            qbs.installSourceBase: "workers"
        }
    }

    //BACKEND TODO
    SubProject {
        filePath: "backend/backend.qbs"
        inheritProperties: true
    }

    //SERVER TODO
    SubProject {
        filePath: "server/server.qbs"
        inheritProperties: true
    }

    SubProject {
        filePath: "app/app.qbs"
        inheritProperties: true
    }
}

import qbs

Project {
    id: dekko
    name: "Dekko Project"
//    minimumQbsVersion: "1.6"

    property string binDir: "bin"
    PropertyOptions {
        name: "binDir"
        description: "Location to install application binaries"
    }

    property string libDir: "lib"
    PropertyOptions {
        name: "libDir"
        description: "Location to install shared libraries"
    }

    property string qmlDir: "lib"
    PropertyOptions {
        name: "qmlDir"
        description: "Location to install QtQuick Plugins. Which can either already \
                      be in the import path or can be picked up via QML2_IMPORT_PATH"
    }

    property string dataDir: "share/dekko"

    property string pluginDir: binDir + "/plugins"

    property bool unity8: false // FIXME: we shouldn't need this anymore.
    PropertyOptions {
        name: "unity8"
        description: "Target platform is unity8"
    }

    property bool click: false

    property bool serverAsThread: false
    PropertyOptions {
        name: "serverAsThread"
        description: "Run the messaging server as a qthread instead of a seperate qprocess"
    }

    property bool enableLogging: true
    PropertyOptions {
        name: "enableLogging"
        description: "Enable qmf internal logging"
    }

    property bool unconfined: false
    PropertyOptions {
        name: "unconfined"
        description: "Allow to run without worrying about platform confinement \
                      - useful for debugging or running on tradition DE's."
    }

    property bool enableUOA: false
    PropertyOptions {
        name: "enableUOA"
        description: "EXPERIMENTAL: enables online accounts integration"
    }

    property bool enableIMAP: true
    PropertyOptions {
        name: "enableIMAP"
        description: "Enables IMAP service"
    }

    property bool enablePOP: false
    PropertyOptions {
        name: "enablePOP"
        description: "Enables POP3 service"
    }

    property bool useUbuntuConnectivity: unity8 ? true : false
    PropertyOptions {
        name: "useUbuntuConnectivity"
    }
//    property bool outputTarBall: false

    property string ui: "ubuntu"
    PropertyOptions {
        name: "ui"
        description: "Which UI should the app be built with"
        allowedValues: ["none", "ubuntu", "qqc2"]
    }

    property bool buildAll: true

    property bool outputTarPackage: false


    qbsSearchPaths: [ path ]

    references: [
        "Dekko/components/components.qbs",
        "Dekko/utils/utils.qbs",
        "upstream/third-party.qbs",
        "Dekko/backend/backend.qbs",
        "Dekko/server/server.qbs",
        "Dekko/app/app.qbs"
    ]

    property string appName: "Dekko"
    property string pkgName: "dekko.dekkoproject"
    property string version: "0.9.6"

    DynamicLibrary {
        name: "API"
        Group {
            name: "Public API"
            files: ["Dekko/api/public/**"]
            fileTags: ["public-qml-api"]
            qbs.install: true
            qbs.installDir: project.qmlDir + "/Dekko/Mail/API"
            qbs.installSourceBase: "Dekko/api/public"
        }

        readonly property stringList qmlImportPaths: [ qbs.installRoot + "/" + project.qmlDir ]


        Group {
            name: "Private API"
            files: ["Dekko/api/private/**"]
            fileTags: ["private-qml-api"]
            qbs.install: true
            qbs.installDir: project.qmlDir + "/Dekko/Mail/API/Private"
            qbs.installSourceBase: "Dekko/api/private"
        }
    }

    DynamicLibrary {
        name: "Stores"

        Group {
            name: "Mail Stores"
            files: ["Dekko/stores/**"]
            fileTags: ["mail-stores"]
            qbs.install: true
            qbs.installDir: project.qmlDir + "/Dekko/Mail/Stores"
            qbs.installSourceBase: "Dekko/stores"
        }
        readonly property stringList qmlImportPaths: [
            qbs.installRoot + "/" + project.qmlDir + "/"
        ]
    }

    DynamicLibrary {
        name: "Workers"

        Group {
            name: "Mail Workers"
            files: ["Dekko/workers/**"]
            fileTags: ["mail-workers"]
            qbs.install: true
            qbs.installDir: project.qmlDir + "/Dekko/Mail/Workers"
            qbs.installSourceBase: "Dekko/workers"
        }
    }

    DynamicLibrary {
        name: "i18n"
        type: "dynamiclibrary"

        Depends { name: "cpp" }
        Depends { name: "Qt.core" }

        Group {
            name: "ts files"
            prefix: "i18n/"
            files: [
                "*.ts"
            ]
        }

        Group {
            qbs.install: true
            qbs.installDir: dekko.binDir + "/i18n"
            fileTagsFilter: ["qm"]
        }
    }

    // If required output a tar.gz package
//    InstallPackage {
//        archiver.type: "tar"
//        targetName: "dekko_" + qbs.architecture + "_qt"+ Qt.core.version
//        name: "tar-package"
//        builtByDefault: project.outputTarPackage

//        Depends { name: "MazDB"; }
//        Depends { name: "LevelDB"; }
//        Depends { name: "MazDBQuick"; }
//        Depends { name: "i18n"; }
//        Depends { name: "Components"; }
//        Depends { name: "Controls"; }
//        Depends { name: "Shared Utils"; }
//        Depends { name: "API"; }
//        Depends { name: "Stores"; }
//        Depends { name: "Workers"; }
//        Depends { name: "Dekko Mail" }
//        Depends { name: "Server" }
//        Depends { name: "Accounts Plugin" }
//        Depends { name: "AccountsLib" }
//        Depends { name: "AutoConfig Plugin" }
//        Depends { name: "Mail Lib" }
//        Depends { name: "Mail Plugin" }
//        Depends { name: "Network Lib" }
//        Depends { name: "Settings Lib" }
//        Depends { name: "Settings Plugin" }
//        Depends { name: "Ubuntu UI Plugin" }
//        Depends { name: "Imap Service"; }
//        Depends { name: "Pop Service"; }
//        Depends { name: "Smtp Service"; }
//        Depends { name: "QmfClient"; }
//        Depends { name: "QmfServer"; }
//        Depends { name: "Settings Service"; }
//        Depends { name: "Storage Manager"; }
//        Depends { name: "Notify Plugin"; }
//        Depends { name: "PlugMan"; }
//        Depends { name: "QuickPlugMan"; }
//        Depends { name: "QuickFlux"; }
//        Depends { name: "Markdown Plugin"; }

//        Group {
//            qbs.install: true
//            fileTagsFilter: product.type
//        }
//    }

    Product {
        name: "Snapcraft"
        type: "packaging"
        Group {
            name: "Snapcraft"
            files: "snap/**"
        }
    }
}

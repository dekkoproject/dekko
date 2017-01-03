import qbs

Project {
    id: dekko
    name: "Dekko Project"

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
        allowedValues: ["ubuntu", "qqc2"]
    }

    property bool buildAll: true

    property bool outputTarPackage: false

    /* MazDB library - LevelDB based database.

       Available as a library and qml plugin
       either use:

       Depend { name: "MazDB" }
       or
       import MazDB 1.0
    */
    SubProject {
        filePath: "upstream/maz-db/MazDB.qbs"
        Properties {
            name: "Maz DB"
            testsEnabled: false
            buildExamples: false
            useSnappy: true
            binDir: dekko.binDir
            libDir: dekko.libDir
            qmlDir: dekko.qmlDir
        }
    }

    /* Quick Flux event framework

       Available as a QML plugin
       import QuickFlux 1.0

       docs: https://quickflux.dekkoproject.org
       upstream: https://github.com/benlau/quickflux
       fork: code.dekkoproject.org/dekko-dev/quick-flux
    */
    SubProject {
        filePath: "upstream/quick-flux/quickflux.qbs"
        Properties {
            name: "Quick Flux"
            staticLib: false
            installDir: dekko.qmlDir + "/QuickFlux"
        }
    }


    SubProject {
        filePath: "upstream/plugman/plugman.qbs"
        Properties {
            libDir: dekko.libDir
            qmlDir: dekko.qmlDir
        }
    }

    SubProject {
        filePath: "upstream/controls/controls.qbs"
        Properties {
            installDir: dekko.qmlDir
        }
    }

    SubProject {
        filePath: "upstream/notify/notify.qbs"
        Properties {
            qmlDir: dekko.qmlDir
        }
    }

    SubProject {
        filePath: "upstream/super-macros/super-macros.qbs"
    }

    SubProject {
        filePath: "upstream/qmf/qmf.qbs"
        Properties {
            libDir: dekko.libDir
            qmfInstallRoot: dekko.libDir + "/qmf"
            enableLogging: dekko.enableLogging
        }
    }

    SubProject {
        filePath: "Dekko/Dekko.qbs"
        inheritProperties: true
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
    InstallPackage {
        archiver.type: "tar"
        targetName: "dekko_" + qbs.architecture + "_qt"+ Qt.core.version
        name: "tar-package"
        builtByDefault: project.outputTarPackage

        Depends { name: "MazDB"; }
        Depends { name: "LevelDB"; }
        Depends { name: "MazDBQuick"; }
        Depends { name: "i18n"; }
        Depends { name: "Components"; }
        Depends { name: "Controls"; }
        Depends { name: "Shared Utils"; }
        Depends { name: "API"; }
        Depends { name: "Stores"; }
        Depends { name: "Workers"; }
        Depends { name: "Dekko Mail" }
        Depends { name: "Server" }
        Depends { name: "Accounts Plugin" }
        Depends { name: "AccountsLib" }
        Depends { name: "AutoConfig Plugin" }
        Depends { name: "Mail Lib" }
        Depends { name: "Mail Plugin" }
        Depends { name: "Network Lib" }
        Depends { name: "Settings Lib" }
        Depends { name: "Settings Plugin" }
        Depends { name : "Ubuntu UI Plugin" }
        Depends { name: "Imap Service"; }
        Depends { name: "Pop Service"; }
        Depends { name: "Smtp Service"; }
        Depends { name: "QmfClient"; }
        Depends { name: "QmfServer"; }
        Depends { name: "Settings Service"; }
        Depends { name: "Storage Manager"; }
        Depends { name: "Notify Plugin"; }
        Depends { name: "PlugMan"; }
        Depends { name: "QuickPlugMan"; }
        Depends { name: "QuickFlux"; }

        Group {
            qbs.install: true
            fileTagsFilter: product.type
        }
    }
}

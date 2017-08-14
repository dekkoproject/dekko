import qbs

Project {
    id: project
    name: "Upstream"

    /* MazDB library - LevelDB based database.

       Available as a library and qml plugin
       either use:

       Depend { name: "MazDB" }
       or
       import MazDB 1.0
    */
    SubProject {
        filePath: "maz-db/MazDB.qbs"
        inheritProperties: true
        Properties {
            name: "Maz DB"
            testsEnabled: false
            buildExamples: false
            useSnappy: true
            binDir: project.binDir
            libDir: project.libDir
            qmlDir: project.qmlDir
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
        filePath: "quick-flux/quickflux.qbs"
        Properties {
            name: "Quick Flux"
            staticLib: false
            installDir: project.qmlDir + "/QuickFlux"
        }
    }


    SubProject {
        filePath: "plugman/plugman.qbs"
        Properties {
            libDir: project.libDir
            qmlDir: project.qmlDir
        }
    }

    SubProject {
        filePath: "controls/controls.qbs"
        Properties {
            installDir: project.qmlDir
        }
    }

    SubProject {
        filePath: "notify/notify.qbs"
        Properties {
            qmlDir: project.qmlDir
        }
    }

    SubProject {
        filePath: "super-macros/super-macros.qbs"
    }

    SubProject {
        filePath: "snap-standard-paths/standardpaths.qbs"
    }


    SubProject {
        filePath: "qmf/qmf.qbs"
        Properties {
            libDir: project.libDir
            qmfInstallRoot: project.libDir + "/qmf"
            enableLogging: project.enableLogging
        }
    }

    SubProject {
        filePath: "ubuntu-plugin/ubuntu-ui.qbs"
        inheritProperties: true
        Properties {
            enabled: project.ui === "ubuntu"
        }
    }
}

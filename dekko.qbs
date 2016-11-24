import qbs

Project {
    id: dekko
    name: "Dekko Project"

    // Binary location
    property string binDir: "bin"
    // library location
    property string libDir: "lib"
    // qml plugin location - i.e QML2_IMPORT_PATH
    property string qmlDir: "lib"

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
            qmlDir: dekko.qmlDir + "/PlugMan"
        }
    }
}

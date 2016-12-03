import qbs
import "./leveldb/LevelDB.qbs" as LevelDB
import "./src/MazDBLib.qbs" as MazDB
import "./src/quick/MazDBQuick.qbs" as MazDBQuick
import "./tests/MazDBTests.qbs" as MazDBTests

Project {
    id: mazdb
    name: "MazDB Project"

    property bool testsEnabled: true
    property bool buildExamples: true
    property bool useSnappy: false
    property string binDir: "bin"
    property string libDir: "lib"
    property string qmlDir: libDir

    LevelDB {
        basePath: "./leveldb"

        Group {
            qbs.install: true
            qbs.installDir: mazdb.libDir
            fileTagsFilter: product.type
        }
    }

    MazDB {
        basePath: "./src"
        Group {
            qbs.install: true
            qbs.installDir: mazdb.libDir
            fileTagsFilter: product.type
        }
    }

    MazDBQuick {
        id: mdbq
        readonly property string pluginDir: mazdb.qmlDir + "/MazDB"
        basePath: "./src/quick"
        Group {
            qbs.install: true
            qbs.installDir: mdbq.pluginDir
            fileTagsFilter: product.type
        }

        Group {
            qbs.install: true
            qbs.installDir: mdbq.pluginDir
            fileTagsFilter: "mazdb-resources"
        }
    }

    QtGuiApplication {
        condition: mazdb.buildExamples
        name: "MazDB Example App"
        targetName: "mazdb-example"
        files: [
            "example/example-qml.qrc",
            "example/example.cpp",
        ]
        Depends { name: "cpp" }
        Depends { name: "Qt.core" }
        Depends { name: "Qt.quick" }
        Depends { name: "Qt.qml" }
        Depends { name: "MazDB" }

        cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
        cpp.debugInformation: qbs.buildVariant === "debug"

        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";
        cpp.includePaths: ["./src/"]

        Group {
            qbs.install: true
            qbs.installDir: mazdb.binDir
            fileTagsFilter: product.type
        }
    }

    MazDBTests {
        condition: mazdb.testsEnabled
    }

    AutotestRunner {
        condition: mazdb.testsEnabled
    }
}

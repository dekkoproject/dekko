import qbs

DynamicLibrary {
    id: mazDBLib

    readonly property var includePaths: ["./src"]
    property string basePath: ""

    name: "MazDB"
    targetName: "maz-db"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.core-private" }
    Depends { name: "Qt.quick" }
    Depends { name: "Qt.qml" }
    Depends { name: "Qt.qml-private" }
    Depends { name: "SnapStandardPaths" }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"

    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: mazDBLib.includePaths

    Export {
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++"; // avoid issues with libc++
        cpp.includePaths: mazDBLib.includePaths
        Depends { name: "cpp"; }
        Depends { name: "Qt.core"; }
        Depends { name: "Qt.quick" }
        Depends { name: "Qt.qml" }
    }

    Depends {
        name: "LevelDB"
    }

    Group {
        name: "C++ Headers"
        prefix: path + "/"
        files: [
            "MazDB.h",
            "MazDBBatch.h",
            "MazOptions.h",
            "MazUtils.h",
            "MazDBListModel.h",
            "MazDBQuery.h",
            "MazDBQueryEngine.h",
            "MazDBSettings.h",
            "MazDBSortProxy.h",
            "QJsEngineShim.h"
        ]
    }

    Group {
        name: "C++ Sources"
        prefix: path + "/"
        files: [
            "MazDB.cpp",
            "MazDBBatch.cpp",
            "MazOptions.cpp",
            "MazUtils.cpp",
            "MazDBListModel.cpp",
            "MazDBQuery.cpp",
            "MazDBQueryEngine.cpp",
            "MazDBSettings.cpp",
            "MazDBSortProxy.cpp"
        ]
    }
}

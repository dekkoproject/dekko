import qbs

DynamicLibrary {
    id: mazDBLib

    readonly property var includePaths: ["./src"]
    property string basePath: ""

    name: "MazDB"
    targetName: "maz-db"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.quick" }
    Depends { name: "Qt.qml" }

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
    }

    Depends {
        name: "LevelDB"
    }

    Group {
        name: "C++ Headers"
        files: [
            basePath + "/MazDB.h",
            basePath + "/MazDBBatch.h",
            basePath + "/MazOptions.h",
            basePath + "/MazUtils.h",
            basePath + "/MazDBListModel.h",
            basePath + "/MazDBQuery.h",
            basePath + "/MazDBQueryEngine.h",
            basePath + "/MazDBSettings.h",
            basePath + "/MazDBSortProxy.h"
        ]
    }

    Group {
        name: "C++ Sources"
        files: [
            basePath + "/MazDB.cpp",
            basePath + "/MazDBBatch.cpp",
            basePath + "/MazOptions.cpp",
            basePath + "/MazUtils.cpp",
            basePath + "/MazDBListModel.cpp",
            basePath + "/MazDBQuery.cpp",
            basePath + "/MazDBQueryEngine.cpp",
            basePath + "/MazDBSettings.cpp",
            basePath + "/MazDBSortProxy.cpp"
        ]
    }
}

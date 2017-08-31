import qbs

DynamicLibrary {
    id: mazDBQuick
    name: "MazDBQuick"
    targetName: "mazdb-plugin"

    readonly property var includePaths: ["./src/quick"]
    property string basePath: ""

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.quick" }
    Depends { name: "Qt.qml" }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"

    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: mazDBQuick.includePaths

    Depends {
        name: "MazDB"
    }

    Group {
        name: "Plugin Headers"
        prefix: path + "/"
        files: [
            "MazDBPlugin.h"
        ]
    }

    Group {
        name: "Plugin Sources"
        prefix: path + "/"
        files: [
            "MazDBPlugin.cpp"
        ]
    }

    Group {
        name: "Other Files"
        prefix: path + "/"
        files: [
            "qmldir"
        ]
        fileTags: ["mazdb-resources"]
    }
}

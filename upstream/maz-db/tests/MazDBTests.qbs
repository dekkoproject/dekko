import qbs

QtGuiApplication {
    id: maztests
    type: ["application", "autotest"]
    name: "MazDB Tests"
    targetName: "mazdb-tests"
    files: [
        path + "/mazdbtests.qrc",
        path + "/runner.cpp",
        path + "/Mace.cpp",
        path + "/Mace.h"
    ]

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.quick" }
    Depends { name: "Qt.qml" }
    Depends { name: "Qt.qmltest" }
    Depends { name: "MazDB" }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"

    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: ["./src/"]

    property string qmlTestsPath: path + "/qml"

    cpp.defines: [
        "QML_TESTS_PATH=\"" + maztests.qmlTestsPath + "\""
    ]
}

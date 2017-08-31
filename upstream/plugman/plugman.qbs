import qbs
import qbs.File
import qbs.FileInfo

Project {
    id: plugman
    name: "Plugin Manager"

    property bool buildExample: false
    property string libDir: "lib"
    property string qmlDir: "lib"

    DynamicLibrary {
        name: "PlugMan"
        targetName: "plugman"

        Depends { name: "cpp" }
        Depends { name: "Qt.core" }
        Depends { name: "Qt.quick" }
        Depends { name: "Qt.qml" }
        Depends { name: "SnapStandardPaths" }

        cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
        cpp.debugInformation: qbs.buildVariant === "debug"
        cpp.includePaths: ["./src"]
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";

        Export {
            Depends { name: "cpp" }
            Depends { name: "Qt.core" }
            Depends { name: "Qt.quick" }
            cpp.includePaths: ["./src"]
            cpp.cxxLanguageVersion: "c++11";
            cpp.cxxStandardLibrary: "libstdc++";
        }

        Group {
            name: "C++ Headers"
            prefix: "src/"
            files: [
                "ActionRegistry.h",
                "ItemRegistry.h",
                "ListenerRegistry.h",
                "PluginIncubator.h",
                "PluginInfo.h",
                "PluginLoader.h",
                "PluginRegistry.h",
                "ServiceRegistry.h"
            ]
        }

        Group {
            name: "C++ Sources"
            prefix: "src/"
            files: [
                "ActionRegistry.cpp",
                "ItemRegistry.cpp",
                "ListenerRegistry.cpp",
                "PluginIncubator.cpp",
                "PluginInfo.cpp",
                "PluginLoader.cpp",
                "PluginRegistry.cpp",
                "ServiceRegistry.cpp",
            ]
        }

        Group {
            qbs.install: true
            qbs.installDir: plugman.libDir
            fileTagsFilter: product.type
        }
    }

    DynamicLibrary {
        name: "QuickPlugMan"
        targetName: "quick-plugman"

        Depends { name: "cpp" }
        Depends { name: "Qt.core" }
        Depends { name: "Qt.quick" }
        Depends { name: "Qt.qml" }
        Depends { name: "PlugMan" }

        cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
        cpp.debugInformation: qbs.buildVariant === "debug"
        cpp.includePaths: ["./src"]
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";


        Group {
            name: "Quick Plugin"
            prefix: "src/quick/"
            files: [
                "QuickPlugger.h",
                "QuickPlugger.cpp"
            ]
        }
        Group {
            name: "Other Files"
            prefix: "src/quick/"
            files: [
                "qmldir"
            ]
            fileTags: ["quickplugman-resources"]
            qbs.install: true
            qbs.installDir: plugman.qmlDir + "/PlugMan"
        }

        Group {
            qbs.install: true
            qbs.installDir: plugman.qmlDir + "/PlugMan"
            fileTagsFilter: product.type
        }
    }

    // EXAMPLES

    DynamicLibrary {
        condition: plugman.buildExample
        name: "Example Plugins"
        targetName: "notify-service"
        Depends { name: "cpp" }
        Depends { name: "Qt.core" }
        Depends { name: "PlugMan" }
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";

        files: [
            "example/NotifySendService.h",
            "example/NotifySendService.cpp"
        ]

        Group {
            qbs.install: true
            qbs.installDir: "bin/plugins"
            fileTagsFilter: product.type
        }

        Group {
            name: "QML Plugins"
            files: ["example/plugins/**"]
            fileTags: ["qml-plugins"]
            qbs.install: true
            qbs.installDir: "bin/plugins"
            qbs.installSourceBase: "example/plugins"
        }
    }


    QtGuiApplication {
        condition: plugman.buildExample
        name: "Example App"
        targetName: "plugman-example"


        Depends { name: "cpp" }
        Depends { name: "Qt.core" }
        Depends { name: "Qt.quick" }
        Depends { name: "Qt.qml" }
        Depends { name: "PlugMan" }
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";

        Group {
            name: "Sources"
            files: [
                "example/main.cpp",
                "example/example.qrc"
            ]
        }

        Group {
            qbs.install: true
            qbs.installDir: "bin"
            fileTagsFilter: product.type
        }
    }
}

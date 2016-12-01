import qbs

Project {
    name: "Dekko Controls"

    // Install location of dynamic library relative to qbs.installRoot
    property string installDir: "lib" // i.e qbs.installRoot + "/lib"
    readonly property string qmlDirName: "Dekko/Controls"

    DynamicLibrary {
        id: controlsLib
        name: "Controls"
        targetName: "controlsplugin"

        readonly property string targetDir: installDir + "/" + qmlDirName

        Depends { name: "cpp" }
        Depends { name: "Qt.core" }
        Depends { name: "Qt.quick" }
        Depends { name: "Qt.qml" }

        cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
        cpp.debugInformation: qbs.buildVariant === "debug"
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";
        cpp.includePaths: ["./src"]

        // Make it easy to depend on this product
        // consumers will just have to use
        // a Depends { name: "QuickFlux" }
        // and not have to worry about this products
        // depends and includes
        Export {
            Depends { name: "cpp" }
            Depends { name: "Qt.core" }
            Depends { name: "Qt.quick" }
            Depends { name: "Qt.qml" }
            cpp.includePaths: ["./src"]
        }

        Group {
            name: "Plugin"
            prefix: "src/"
            files: [
                "ControlsPlugin.h",
                "ControlsPlugin.cpp",
                "QQuickStack.h",
                "QQuickStack.cpp"
            ]
        }

        Group {
            qbs.install: true
            qbs.installDir: controlsLib.targetDir
            fileTagsFilter: product.type
        }

        Group {
            name: "QML Files"
            prefix: "src/"
            files: ["*.qml", "*.js", "qmldir"]
            fileTags: ["qml-plugins"]
            qbs.install: true
            qbs.installDir: controlsLib.targetDir
        }
    }
}

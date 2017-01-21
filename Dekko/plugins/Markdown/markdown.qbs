import qbs

Project {
    name: "Markdown"

    DynamicLibrary {
        name: "Markdown Plugin"
        targetName: "markdownplugin"

        Depends { name: "cpp" }
        Depends { name: "SuperMacros" }
        Depends {
            name: "Qt"
            submodules: [
                "core",
                "quick",
                "qml"
            ]
        }

        cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
        cpp.debugInformation: qbs.buildVariant === "debug"
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";
        cpp.includePaths: [ path ]

        Group {
            name: "Python Modules"
            files: "*.py"
            fileTags: ["md-plugin"]
        }

        Group {
            name: "QML components"
            files: [
                "*.qml",
            ]
            fileTags: ["md-plugin"]
        }

        Group {
            name: "QML directory"
            files: "qmldir"
            fileTags: ["md-plugin"]
        }

        Group {
            fileTagsFilter: "md-plugin"
            qbs.install: true
            qbs.installDir: project.qmlDir + "/Dekko/" + project.name
        }

        Group {
            name: "C++ Headers"
            files: "*.h"
        }

        Group {
            name: "C++ Sources"
            files: "*.cpp"
        }

        Group {
            fileTagsFilter: product.type
            qbs.install: true
            qbs.installDir: project.qmlDir + "/Dekko/" + project.name
        }
    }
}

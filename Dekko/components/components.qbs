import qbs

DynamicLibrary {
    name: "Components"
    targetName: "componentsplugin"

    Depends { name: "cpp" }
    Depends { name: "Shared Utils" }
    Depends {
        name: "Qt"
        submodules: [
            "svg"
        ]
    }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [ path ]

    Export {
        Depends { name: "cpp" }
        Depends { name: "Shared Utils" }
        Depends {
            name: "Qt"
            submodules: [
                "svg"
            ]
        }
        cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
        cpp.debugInformation: qbs.buildVariant === "debug"
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";
        cpp.includePaths: [ path ]
    }

    Group {
        name: "C++ Sources"
        prefix: path + "/"
        files: [
            "*.cpp"
        ]
    }

    Group {
        name: "C++ Headers"
        prefix: path + "/"
        files: [
            "*.h"
        ]
    }

    Group {
        name: "QML directory"
        prefix: path + "/"
        files: [
            "qmldir"
        ]
        fileTags: ["comp-resources"]
    }

    Group {
        qbs.install: true
        qbs.installDir: project.qmlDir + "/Dekko/Components"
        fileTagsFilter: product.type
    }

    Group {
        qbs.install: true
        qbs.installDir: project.qmlDir + "/Dekko/Components"
        fileTagsFilter: "comp-resources"
    }
}

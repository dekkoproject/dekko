import qbs

DynamicLibrary {

    name: "Shared Utils"
    targetName: "dekko-utils"

    Depends { name: "cpp" }
    Depends {
        name: "Qt"
        submodules: [
            "core",
            "quick",
            "gui"
        ]
    }
    Depends { name: "SnapStandardPaths" }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [ path ]

    Export {
        Depends { name: "cpp" }
        Depends {
            name: "Qt"
            submodules: [
                "core",
                "quick",
                "gui"
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
        qbs.install: true
        qbs.installDir: project.libDir
        fileTagsFilter: product.type
    }

    Group {
        name: "JS Helpers"
        prefix: path + "/js/"
        files: ["*.js", "qmldir"]
        fileTags: ["js-utils"]
        qbs.install: true
        qbs.installDir: project.qmlDir + "/Dekko/Utils"
    }
}

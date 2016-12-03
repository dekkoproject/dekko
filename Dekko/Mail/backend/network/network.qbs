import qbs

DynamicLibrary {
    id: network

    name: "Network Lib"
    targetName: "network"

    Depends { name: "cpp" }
    Depends { name: "Shared Utils" }
    Depends { name: "Mail Lib" }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [ path ]

    Export {
        Depends { name: "cpp" }
        Depends { name: "Shared Utils" }
        Depends { name: "Mail Lib" }

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
}

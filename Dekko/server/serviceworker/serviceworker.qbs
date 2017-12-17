import qbs

CppApplication {
    name: "Service Worker"
    targetName: "dekko-worker"

    Depends { name: "Qt.core" }
    Depends { name: "Mail Lib" }
    Depends { name: "AccountsLib" }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [ path ]

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
        qbs.installDir: project.binDir
        fileTagsFilter: product.type
    }
}

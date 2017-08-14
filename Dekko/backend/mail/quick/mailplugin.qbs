import qbs

DynamicLibrary {
    name: "Mail Plugin"
    targetName: "mail-plugin"

    Depends { name: "cpp" }
    Depends {
        name: "Qt"
        submodules: [
            "quick",
            "qml"
        ]
    }
    Depends { name: "QmfServer" }
    Depends { name: "AccountsLib" }
    Depends { name: "Shared Utils" }
    Depends { name: "SuperMacros" }
    Depends { name: "Mail Lib" }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [ path ]

    cpp.defines: {
    }

    Group {
        name: "C++ Sources"
        prefix: path + "/"
        files: [
            "MailPlugin.cpp",
            "qmlenums.cpp"
        ]
    }

    Group {
        name: "C++ Headers"
        prefix: path + "/"
        files: [
            "MailPlugin.h",
            "qmlenums.h"
        ]
    }

    Group {
        name: "Other Files"
        prefix: path + "/"
        files: [
            "qmldir"
        ]
        fileTags: ["mail-resources"]
        qbs.install: true
        qbs.installDir: project.qmlDir + "/Dekko/Mail"
    }

    Group {
        qbs.install: true
        qbs.installDir: project.qmlDir + "/Dekko/Mail"
        fileTagsFilter: product.type
    }
}

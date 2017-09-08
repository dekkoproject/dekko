import qbs

DynamicLibrary {
    name: "Accounts Plugin"
    targetName: "accounts-plugin"

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

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [ path ]
    cpp.defines: [
        "IMAP_ENABLED",
        "POP3_ENABLED"
    ]

    Group {
        name: "C++ Sources"
        prefix: path + "/"
        files: [
            "AccountsPlugin.cpp",
            "PresetProviderModel.cpp"
        ]
    }

    Group {
        name: "C++ Headers"
        prefix: path + "/"
        files: [
            "AccountsPlugin.h",
            "PresetProviderModel.h"
        ]
    }

    Group {
        name: "Other Files"
        prefix: path + "/"
        files: [
            "qmldir"
        ]
        fileTags: ["account-resources"]
        qbs.install: true
        qbs.installDir: project.qmlDir + "/Dekko/Mail/Accounts"
    }

    Group {
        qbs.install: true
        qbs.installDir: project.qmlDir + "/Dekko/Mail/Accounts"
        fileTagsFilter: product.type
    }
}

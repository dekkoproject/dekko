import qbs

DynamicLibrary {
    name: "Settings Plugin"
    targetName: "settings-plugin"

    Depends { name: "cpp" }
    Depends {
        name: "Qt"
        submodules: [
            "quick",
            "qml"
        ]
    }
    Depends { name: "QmfClient" }
    Depends { name: "AccountsLib" }
    Depends { name: "Shared Utils" }
    Depends { name: "Settings Lib" }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [ path ]

    Group {
        name: "C++ Sources"
        prefix: path + "/"
        files: [
            "SettingsPlugin.cpp"
        ]
    }

    Group {
        name: "C++ Headers"
        prefix: path + "/"
        files: [
            "SettingsPlugin.h"
        ]
    }

    Group {
        name: "Other Files"
        prefix: path + "/"
        files: [
            "qmldir"
        ]
        fileTags: ["settings-resources"]
        qbs.install: true
        qbs.installDir: project.qmlDir + "/Dekko/Mail/Settings"
    }

    Group {
        qbs.install: true
        qbs.installDir: project.qmlDir + "/Dekko/Mail/Settings"
        fileTagsFilter: product.type
    }
}

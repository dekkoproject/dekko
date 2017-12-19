import qbs

DynamicLibrary {
    name: "AccountsLib"
    targetName: "accounts"

    Depends { name: "cpp" }
    Depends { name: "QmfClient" }
    Depends { name: "QmfServer" }
    Depends { name: "Shared Utils" }
    Depends { name: "MazDB" }
    Depends { name: "SnapStandardPaths" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.dbus" }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [ path, path + "/../utils/" ]

    Properties {
        condition: project.click
        cpp.defines: [
            "NO_TEMPLATE_STREAM"
        ]
    }

    Export {
        Depends { name: "cpp" }
        Depends { name: "QmfClient" }
        Depends { name: "QmfServer" }
        Depends { name: "Shared Utils" }
        Depends { name: "MazDB" }
        Depends { name: "Qt.core" }
        Depends { name: "Qt.dbus" }
        cpp.includePaths: [ path, path + "/../utils/" ]
    }

    Group {
        name: "Accounts Library"
        prefix: path + "/"
        files: ["*.h", "*.cpp"]
    }

    Group {
        name: "Accounts Service"
        prefix: path + "/service/"
        files: ["*.h", "*.cpp", "dbus/*.xml"]
    }

    Group {
        qbs.install: true
        qbs.installDir: project.libDir
        fileTagsFilter: product.type
    }
}


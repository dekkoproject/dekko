import qbs

DynamicLibrary {
    name: "Mail Lib"
    targetName: "mail"

    Depends { name: "cpp" }
    Depends { name: "AccountsLib" }
    Depends { name: "Settings Lib" }
    Depends { name: "Qt.dbus" }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [ path,  path + "/service/", path + "/../utils/"]

    Properties {
        condition: project.click
        cpp.defines: [
            "NO_TEMPLATE_STREAM"
        ]
    }

    Export {
        Depends { name: "cpp" }
        Depends { name: "AccountsLib" }
        Depends { name: "Settings Lib" }
        Depends { name: "Qt.dbus" }

        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";
        cpp.includePaths: [ path,  path + "/service/", path + "/../utils/"]
    }

    Group {
        name: "Mail Library"
        prefix: path + "/"
        files: ["*.h", "*.cpp"]
    }

    Group {
        name: "Mail Service"
        prefix: path + "/service/"
        files: ["*.h", "*.cpp", "dbus/*.xml"]
    }

    Group {
        qbs.install: true
        qbs.installDir: project.libDir
        fileTagsFilter: product.type
    }
}

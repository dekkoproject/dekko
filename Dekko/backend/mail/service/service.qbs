import qbs

DynamicLibrary {
    name: "Mail Service"
    targetName: "libmailservice"

    Depends { name: "cpp" }
    Depends { name: "Shared Utils" }
    Depends { name: "QmfClient" }
    Depends { name: "QmfServer" }
    Depends { name: "Qt.dbus" }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [ path ]

    Export {
        Depends { name: "cpp" }
        Depends { name: "QmfClient" }
        Depends { name: "QmfServer" }
        Depends { name: "Shared Utils" }
        Depends { name: "Qt.dbus" }

        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";
        cpp.includePaths: [ path ]
    }

    Group {
        name: "XML Interfaces"
        prefix: path + "/dbus/"
        files: ["*.xml"]
    }

    Group {
        name: "C++ Headers"
        prefix: path + "/"
        files: ["*.h"]
    }

    Group {
        name: "C++ Sources"
        prefix: path + "/"
        files: ["*.cpp"]
    }

    Group {
        qbs.install: true
        qbs.installDir: project.libDir
        fileTagsFilter: product.type
    }
}

import qbs

DynamicLibrary {
    name: "Pop Service"
    targetName: "pop"

    Depends { name: "cpp" }
    Depends { name: "QmfClient" }
    Depends { name: "QmfServer" }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [
        path + "/pop/"
    ]

    Properties {
        condition: project.enableLogging
        cpp.defines: ["QMF_NO_MESSAGE_SERVICE_EDITOR", "QMF_ENABLE_LOGGING"]
    }

    Properties {
        condition: !project.enableLogging
        cpp.defines: ["QMF_NO_MESSAGE_SERVICE_EDITOR"]
    }

    Group {
        name: "C++ Sources"
        prefix: path + "/pop/"
        files: [
            "popclient.cpp",
            "popconfiguration.cpp",
            "popservice.cpp",
            "popauthenticator.cpp"
        ]
    }

    Group {
        name: "C++ Headers"
        prefix: path + "/pop/"
        files: ["*.h"]
        excludeFiles: ["popsettings.h"]
    }

    Group {
        qbs.install: true
        qbs.installDir: project.qmfInstallRoot + "/plugins5/messageservices"
        fileTagsFilter: product.type
    }
}

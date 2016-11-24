import qbs

DynamicLibrary {
    name: "Settings Service"
    targetName: "qmfsettings"

    Depends { name: "cpp" }
    Depends { name: "QmfClient" }
    Depends { name: "QmfServer" }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [
        "plugins/messageservices/qmfsettings/"
    ]

    cpp.defines: {
        var defs = ["QMF_NO_MESSAGE_SERVICE_EDITOR"]
        if (project.enableLogging) {
            defs.concat("QMF_ENABLE_LOGGING")
        }

        if (project.uoaEnabled) {
            defs.concat("UOA_ENABLED")
        }
        return defs
    }

    Group {
        name: "C++ Sources"
        prefix: "plugins/messageservices/qmfsettings/"
        files: [
            "service.cpp"
        ]
    }

    Group {
        name: "C++ Headers"
        prefix: "plugins/messageservices/qmfsettings/"
        files: ["service.h"]
    }

    Group {
        qbs.install: true
        qbs.installDir: project.qmfInstallRoot + "/plugins5/messageservices"
        fileTagsFilter: product.type
    }
}

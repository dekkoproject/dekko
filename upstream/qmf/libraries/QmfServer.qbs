import qbs

Product {
    name: "QmfServer"
    targetName: "qmfmessageserver5"
    type: "dynamiclibrary"

    Depends { name: "cpp" }
    Depends { name: "QmfClient" }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [
        path + "/qmfmessageserver"
    ]
    property stringList defaultDefines: [
        "QMF_NO_MESSAGE_SERVICE_EDITOR",
        "MESSAGESERVER_INTERNAL",
        "QMF_ENABLE_LOGGING",
        "SNAP"
    ]
    cpp.defines: defaultDefines

    Export {
        Depends { name: "cpp" }
        Depends { name: "Qt.core" }
        Depends { name: "Qt.gui" }
        Depends { name: "Qt.network" }
        Depends { name: "Qt.sql" }
        Depends { name: "QmfClient" }
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";
        cpp.includePaths: [
            path + "/qmfmessageserver"
        ]
    }

    Group {
        name: "C++ Source"
        prefix: path + "/qmfmessageserver/"
        files: [
            "qmailauthenticator.cpp",
            "qmailmessagebuffer.cpp",
            "qmailmessageclassifier.cpp",
            "qmailmessageservice.cpp",
            "qmailserviceconfiguration.cpp",
            "qmailstoreaccountfilter.cpp",
            "qmailtransport.cpp",
            "qmailheartbeattimer_qtimer.cpp",
            "qmailmessageserverplugin.cpp"
        ]
    }

    Group {
        name: "C++ Headers"
        prefix: path + "/qmfmessageserver/"
        files: ["*.h"]
    }

    Group {
        qbs.install: true
        qbs.installDir: project.libDir
        fileTagsFilter: product.type
    }

}

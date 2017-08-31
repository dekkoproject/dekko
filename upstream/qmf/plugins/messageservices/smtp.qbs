import qbs
import qbs.Probes as Probes

DynamicLibrary {
    name: "Smtp Service"
    targetName: "smtp"

    Probes.PkgConfigProbe {
        id: connectivityapi
        condition: project.unity8
        name: "connectivity-qt1"
    }

    Depends { name: "cpp" }
    Depends { name: "QmfClient" }
    Depends { name: "QmfServer" }
    Depends {
        name: "Qt"
        submodules: [
            "core",
            "dbus",
            "network",
            "sql",
            "xml"
        ]
    }

    cpp.linkerFlags: {
        var libs = [];
        if (project.unity8 && connectivityapi.found) {
            libs.concat(connectivityapi.libs)
        }
        return libs
    }

    cpp.cxxFlags: {
        var flags = []
        if (project.unity8 && connectivityapi.found) {
            flags.concat(connectivityapi.cflags)
        }
        return flags
    }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [
        path + "/smtp/"
    ]

    Properties {
        condition: project.enableLogging
        cpp.defines: ["PLUGIN_INTERNAL", "QMF_NO_MESSAGE_SERVICE_EDITOR", "QMF_ENABLE_LOGGING"]
    }

    Properties {
        condition: !project.enableLogging
        cpp.defines: ["PLUGIN_INTERNAL", "QMF_NO_MESSAGE_SERVICE_EDITOR"]
    }

    Group {
        name: "C++ Sources"
        prefix: path + "/smtp/"
        files: [
            "smtpauthenticator.cpp",
            "smtpclient.cpp",
            "smtpconfiguration.cpp",
            "smtpservice.cpp"
        ]
    }

    Group {
        name: "C++ Headers"
        prefix: path + "/smtp/"
        files: [
            "smtpauthenticator.h",
            "smtpclient.h",
            "smtpconfiguration.h",
            "smtpservice.h"
        ]
    }

    Group {
        qbs.install: true
        qbs.installDir: project.qmfInstallRoot + "/plugins5/messageservices"
        fileTagsFilter: product.type
    }
}

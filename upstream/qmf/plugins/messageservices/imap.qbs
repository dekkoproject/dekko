import qbs
import qbs.Probes as Probes

DynamicLibrary {
    id: imap
    name: "Imap Service"
    targetName: "imap"

    Probes.PkgConfigProbe {
        id: zlib
        name: "zlib"
    }

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
            "sql"
        ]
    }

    cpp.linkerFlags: {
        var libs = [];
        if (zlib.found) {
            libs.concat(zlib.libs)
        }
        if (project.unity8 && connectivityapi.found) {
            libs.concat(connectivityapi.libs)
        }
        return libs
    }

    cpp.cxxFlags: {
        var flags = []
        if (zlib.found) {
            flags.concat(zlib.cflags)
        }
        if (project.unity8 && connectivityapi.found) {
            flags.concat(connectivityapi.cflags)
        }
        return flags
    }

    cpp.dynamicLibraries: zlib.found ? ["z"] : []
    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [
        path + "/imap/"
    ]

    Properties {
        condition: project.enableLogging
        cpp.defines: ["QT_QMF_HAVE_ZLIB", "QMF_NO_MESSAGE_SERVICE_EDITOR", "QMF_ENABLE_LOGGING"]
    }

    Properties {
        condition: !project.enableLogging
        cpp.defines: ["QT_QMF_HAVE_ZLIB", "QMF_NO_MESSAGE_SERVICE_EDITOR"]
    }

    Group {
        name: "C++ Sources"
        prefix: path + "/imap/"
        files: [
            "imapclient.cpp",
            "imapconfiguration.cpp",
            "imapprotocol.cpp",
            "imapservice.cpp",
            "imapstructure.cpp",
            "imapauthenticator.cpp",
            "imapstrategy.cpp",
            "integerregion.cpp",
            "imaptransport.cpp",
            "serviceactionqueue.cpp"
        ]
    }

    Group {
        name: "C++ Headers"
        prefix: path + "/imap/"
        files: ["*.h"]
        excludeFiles: ["imapsettings.h"]
    }

    Group {
        qbs.install: true
        qbs.installDir: project.qmfInstallRoot + "/plugins5/messageservices"
        fileTagsFilter: product.type
    }
}

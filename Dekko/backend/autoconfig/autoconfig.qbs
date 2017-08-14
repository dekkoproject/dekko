import qbs
import qbs.Probes as Probes

DynamicLibrary {
    id: autoConfig
    name: "AutoConfig Plugin"
    targetName: "autoconfig"

    readonly property string installDir: project.qmlDir + "/Dekko/AutoConfig"

    Probes.PkgConfigProbe {
        id: connectivity
        name: "connectivity-qt1"
    }

    Depends { name: "cpp" }
    Depends { name: "SuperMacros" }
    Depends { name: "Shared Utils" }
    Depends {
        name: "Qt"
        submodules: [
            "core",
            "quick",
            "qml",
            "network",
            "xml"
        ]
    }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [ path ]

    property stringList pkgLibs: []
    cpp.linkerFlags: {
        if (project.useUbuntuConnectivity && connectivity.found) {
            autoConfig.pkgLibs.concat(connectivity.libs)
        }
        return autoConfig.pkgLibs
    }

    property stringList pkgFlags: []
    cpp.cxxFlags: {
        if (project.useUbuntuConnectivity && connectivity.found) {
            autoConfig.pkgFlags.concat(connectivity.cflags)
        }
        return autoConfig.pkgFlags;
    }

    cpp.defines: {
        var defs = []
        if (project.useUbuntuConnectivity && connectivity.found) {
            defs.concat("USE_CONNECTIVITY_API")
        }
        return defs
    }

    Group {
        name: "C++ Sources"
        prefix: path + "/"
        files: [
            "emailprovider.cpp",
            "autoconfigplugin.cpp",
            "autoconfig.cpp",
            "autodiscover.cpp",
            "serverconfiguration.cpp",
            "srvlookup.cpp"
        ]
    }

    Group {
        name: "C++ Headers"
        prefix: path + "/"
        files: [
            "emailprovider.h",
            "autoconfigplugin.h",
            "autoconfig.h",
            "autodiscover.h",
            "serverconfiguration.h",
            "srvlookup.h"
        ]
    }

    Group {
        name: "Other Files"
        prefix: path + "/"
        files: [
            "qmldir"
        ]
        fileTags: ["autoconfig-resources"]
        qbs.install: true
        qbs.installDir: autoConfig.installDir
    }

    Group {
        qbs.install: true
        qbs.installDir: autoConfig.installDir
        fileTagsFilter: product.type
    }
}

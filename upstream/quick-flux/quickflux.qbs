import qbs

Project {
    id: quickflux

    name: "QuickFlux Project"

    // e.g qbs profile:<profile> build ... project.staticLib:true
    property bool staticLib: false
    // Install location of dynamic library relative to qbs.installRoot
    property string installDir: "lib/QuickFlux" // i.e qbs.installRoot + "/lib"

    Product {
        name: "QuickFlux"
        targetName: "quickflux"
        type: quickflux.staticLib ? "staticlibrary" : "dynamiclibrary"

        Depends { name: "cpp" }
        Depends { name: "Qt.core" }
        Depends { name: "Qt.quick" }
        Depends { name: "Qt.qml" }

        cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
        cpp.debugInformation: qbs.buildVariant === "debug"
        
        cpp.includePaths: ["./quickflux", "./quickflux/priv"]
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";

        // Make it easy to depend on this product
        // consumers will just have to use
        // a Depends { name: "QuickFlux" }
        // and not have to worry about this products
        // depends and includes
        Export {
            Depends { name: "cpp" }
            Depends { name: "Qt.core" }
            Depends { name: "Qt.quick" }
            Depends { name: "Qt.qml" }
            cpp.includePaths: ["./quickflux", "./quickflux/priv"]
            cpp.cxxLanguageVersion: "c++11";
            cpp.cxxStandardLibrary: "libstdc++";
        }

        Group {
            name: "Public Headers"
            prefix: "quickflux/"
            files: [
                "qfappdispatcher.h",
                "qfapplistener.h",
                "qfappscript.h",
                "qfapplistenergroup.h",
                "qfappscriptgroup.h",
                "qffilter.h",
                "qfkeytable.h",
                "qfactioncreator.h",
                "QFAppDispatcher",
                "QFKeyTable",
                "QuickFlux"
            ]
        }

        Group {
            name: "Public Sources"
            prefix: "quickflux/"
            files: [
                "qfapplistener.cpp",
                "qfappdispatcher.cpp",
                "qfappscript.cpp",
                "qfappscriptrunnable.cpp",
                "qfappscriptdispatcherwrapper.cpp",
                "qflistener.cpp",
                "qfapplistenergroup.cpp",
                "qfappscriptgroup.cpp",
                "qffilter.cpp",
                "qfkeytable.cpp",
                "qfactioncreator.cpp"
            ]
        }

        Group {
            name: "Private Headers"
            prefix: "quickflux/priv/"
            files: [
                "qfappscriptdispatcherwrapper.h",
                "qfappscriptrunnable.h",
                "qflistener.h",
                "qfsignalproxy.h",
            ]
        }

        Group {
            name: "Private Sources"
            prefix: "quickflux/priv/"
            files: [
                "qfsignalproxy.cpp"
            ]
        }

        // For the dynamic library. Apps can pick up
        // the plugin on the QML2_IMPORT_PATH
        Group {
            name: "QuickFlux Plugin"
            condition: !quickflux.staticLib
            files: [
                "quickflux_plugin.cpp",
                "quickflux_plugin.h"
            ]
        }

        Group {
            condition: !quickflux.staticLib
            name: "Other Files"
            files: [
                "qmldir"
            ]
            fileTags: ["quickflux-resources"]
            qbs.install: true
            qbs.installDir: quickflux.installDir
        }

        Group {
            condition: !quickflux.staticLib
            qbs.install: true
            qbs.installDir: quickflux.installDir
            fileTagsFilter: product.type
        }
    }
}

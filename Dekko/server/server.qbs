import qbs

Project {
    name: "Server"

    references: [
        "serviceplugin/dekkod-service.qbs"
    ]

    QtGuiApplication {
        name: "Server"
        targetName: "dekkod"

        Depends { name: "cpp" }
        Depends {
            name: "Qt"
            submodules: [
                "core",
                "gui",
                "widgets"
            ]
        }
        Depends { name: "QmfClient" }
        Depends { name: "QmfServer" }

        cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
        cpp.debugInformation: qbs.buildVariant === "debug"
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";
        cpp.includePaths: [ path ]
        cpp.defines: {
            var defs = ["SNAP", "QMF_NO_MESSAGE_SERVICE_EDITOR"]
            if (project.unity8) {
                defs.concat("IS_UNITY8")
            } else {
                defs.concat("USE_HTML_PARSER")
            }
            if (project.click) {
                defs.concat("CLICK")
            }
            if (project.enableLogging) {
                defs.concat("QMF_ENABLE_LOGGING")
            }
            if (project.uoaEnabled) {
                defs.concat("USE_ACCOUNTS_QT")
            }
            return defs
        }

        Group {
            name: "C++ Sources"
            prefix: path + "/"
            files: [
                "*.cpp"
            ]
            excludeFiles: project.serverAsThread ? ["main.cpp"] : []
        }

        Group {
            name: "C++ Headers"
            prefix: path + "/"
            files: [
                "*.h"
            ]
        }

        Group {
            qbs.install: true
            qbs.installDir: project.binDir
            fileTagsFilter: product.type
        }
    }
}

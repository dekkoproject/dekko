import qbs

Project {
    name: "Server"

    references: [
        "serviceplugin/dekkod-service.qbs",
        "serviceworker/serviceworker.qbs"
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
        cpp.defines: [
            "SNAP",
            "QMF_NO_MESSAGE_SERVICE_EDITOR",
            "HAVE_LIBICU",
            "USE_HTML_PARSER",
            "QMF_ENABLE_LOGGING"
        ]

        Properties {
            condition: project.click
            cpp.defines: [
                "CLICK",
                "QMF_NO_MESSAGE_SERVICE_EDITOR",
                "HAVE_LIBICU",
                "QMF_ENABLE_LOGGING"
            ]
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

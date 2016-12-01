import qbs
import "ubuntu/ubuntu-ui.qbs" as UbuntuUI
import "qqc2/qqc2-ui.qbs" as QQC2UI

Project {

    name: "App"

    QtGuiApplication {
        name: "Dekko Mail"
        targetName: "dekko-mail"

        Depends { name : "cpp" }
        Depends {
            name: "Qt"
            submodules: [
                "core",
                "quick",
                "qml",
                "gui",
                "widgets"
            ]
        }
        Depends { name: "QmfClient" }
        Depends { name: "QmfServer" }
        Depends { name: "PlugMan" }
        Depends { name: "Network Lib" }

        cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
        cpp.debugInformation: qbs.buildVariant === "debug"
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";
        cpp.includePaths: [ path ]

        cpp.defines: [
            "SNAP_MODE",
            "DEKKO_VERSION=\"" + project.version + "\""
        ]


            /*{
            var defs = []
            defs.concat("SNAP_MODE")
            defs.concat("DEKKO_VERSION=\"0.9.4\"")
            if (project.unity8) {
                defs.concat("IS_UNITY8")
            }
            if (project.enableLogging) {
                defs.concat("QMF_ENABLE_LOGGING")
            }
            if (project.serverAsThread) {
                defs.concat("SERVER_AS_QTHREAD")
            }
            return defs
        }*/

        UbuntuUI {

        }

        QQC2UI {

        }

        Group {
            name: "C++ Sources"
            files: [
                "*.cpp"
            ]
        }

        Group {
            name: "C++ Headers"
            files: [
                "*.h"
            ]
        }

        Group {
            name: "Asset Configs"
            files: ["assets/configuration/**"]
            qbs.install: true
            qbs.installDir: "/usr/share/dekko"
            qbs.installSourceBase: "assets/configuration"
        }

        Group {
            name: "Asset Resources"
            files: ["assets/**/*.qrc"]
        }

        Group {
            name: "QML Resources"
            files: ["dekko.qrc"]
        }

        Group {
            qbs.install: true
            qbs.installDir: project.binDir
            fileTagsFilter: product.type
        }
    }
}

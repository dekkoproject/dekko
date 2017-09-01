import qbs
import qbs.Process

Project {

    name: "App"

    QtGuiApplication {
        name: "Dekko Mail"
        targetName: "dekko"

        Probe {
            id: revprobe
            property string revision
            // --git-dir needs to be pointed at the toplevel .git directory to work
            property string sourceDir: project.sourceDirectory + "/.git"

            configure: {
                revision = "no-git";
                var p = new Process();
                p.setWorkingDirectory(sourceDir)
                if (p.exec("/usr/bin/git", ["--git-dir=" + sourceDir, "describe", "--dirty", "--long", "--always"], true) === 0)
                    revision = p.readStdOut().trim();
            }
        }

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
        Depends { name: "SnapStandardPaths" }

        cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
        cpp.debugInformation: qbs.buildVariant === "debug"
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";
        cpp.includePaths: [ path ]

        cpp.defines: [
            "SNAP_MODE",
            "APP_NAME=\"dekko\"",
            "APP_ORG=\"dekkoproject\"",
            "DEKKO_VERSION=\"" + project.version + "-" + revprobe.revision + "\""
        ]

        Properties {
            condition: project.click
            cpp.defines: [
                "CLICK_MODE",
                "APP_NAME=\"dekko2.dekkoproject\"",
                "APP_ORG=\"dekko2.dekkoproject\"",
                "DEKKO_VERSION=\"" + project.version + "-" + revprobe.revision + "\""
            ]
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
            files: [
                "assets/configuration/**"
            ]
            qbs.install: true
            qbs.installDir: "/usr/share/dekko/configuration"
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

import qbs
import qbs.Process

Project {
    name: "PyOtherside"

    property string pyothersideInstallDir: "lib/io/thp/pyotherside"
    property bool usePreBuilt: false


    DynamicLibrary {
        name: "pyotherside"
        targetName: "pyothersideplugin"
        builtByDefault: !project.usePreBuilt

        Probe {
            id: py_config

            property stringList ldflags
            property stringList includes

            configure: {
                var p = new Process();
                try {
                    p.setWorkingDirectory(path)

                    if (p.exec("/usr/bin/python3-config", ["--ldflags"] , true) === 0) {
                        ldflags = p.readStdOut().trim();
                        ldflags = ldflags ? ldflags.split(/\s/) : [];
                    } else {
                        throw "Unable to get python3-config ldflags"
                    }
                    if (p.exec("/usr/bin/python3-config", ["--includes"] , true) === 0) {
                        includes = p.readStdOut().trim();
                        includes = includes ? includes.split(/\s/) : [];
                    } else {
                        throw "Unable to get python3-config includes"
                    }
                } finally {
                    p.close()
                }
            }
        }

        Depends { name: "cpp" }
        Depends { name: "Qt.core" }
        Depends { name: "Qt.quick" }
        Depends { name: "Qt.qml" }
        Depends { name: "Qt.svg" }

        cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
        cpp.debugInformation: qbs.buildVariant === "debug"

        cpp.includePaths: [ path + "/src" ]
        cpp.dynamicLibraries: ["python3.5m", "dl"]
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";
        cpp.defines: [
            "HAVE_DLADDR",
            "PYOTHERSIDE_VERSION=\"1.5.1\""
        ]
        cpp.linkerFlags: py_config.ldflags
        cpp.cxxFlags: py_config.includes

        Group {
            name: "PyOtherside QML Plugin"
            prefix: path + "/src/"
            files: [
                "pyotherside_plugin.cpp",
                "pyotherside_plugin.h"
            ]
        }

        Group {
            name: "PyGLArea"
            prefix: path + "/src/"
            files: [
                "pyglarea.cpp",
                "pyglarea.h",
                "pyglrenderer.cpp",
                "pyglrenderer.h"
            ]
        }

        Group {
            name: "QML Image Provider"
            prefix: path + "/src/"
            files: [
                "qpython_imageprovider.cpp",
                "qpython_imageprovider.h"
            ]
        }

        Group {
            name: "PyFBO"
            prefix: path + "/src/"
            files: [
                "pyfbo.cpp",
                "pyfbo.h"
            ]
        }

        Group {
            name: "Python Loader"
            prefix: path + "/src/"
            files: [
                "pythonlib_loader.cpp",
                "pythonlib_loader.h",
                "global_libpython_loader.cpp",
                "global_libpython_loader.h"
            ]
        }

        Group {
            name: "Python QML Object"
            prefix: path + "/src/"
            files: [
                "qpython.cpp",
                "qpython.h",
                "qpython_worker.cpp",
                "qpython_worker.h",
                "qpython_priv.cpp",
                "qpython_priv.h"
            ]
        }

        Group {
            name: "Object Wrappers"
            prefix: path + "/src/"
            files: [
                "pyobject_ref.cpp",
                "pyobject_ref.h",
                "qobject_ref.cpp",
                "qobject_ref.h",
                "pyqobject.h",
                "ensure_gil_state.h"
            ]
        }

        Group {
            name: "Type Conversions"
            prefix: path + "/src/"
            files: [
                "converter.h",
                "qvariant_converter.h",
                "pyobject_converter.h",
                "qml_python_bridge.h"
            ]
        }

        Group {
            condition: product.builtByDefault
            name: "Other Files"
            files: [
                path + "/src/qmldir",
//                path + "./src/pyotherside.qmltypes"
            ]
            fileTags: ["pyotherside-resources"]
            qbs.install: true
            qbs.installDir: project.pyothersideInstallDir
        }

        Group {
            condition: product.builtByDefault
            qbs.install: true
            qbs.installDir: project.pyothersideInstallDir
            fileTagsFilter: product.type
        }
    }

    Product {
        name: "prebuilt-pyotherside"
        type: "prebuilt"
        builtByDefault: project.usePreBuilt

        Group {
            name: "Vivid 15.04 armhf"
            condition: product.builtByDefault
            files: [
                path + "/prebuilt/qmldir",
                path + "/prebuilt/*.so",
                path + "/prebuilt/*.qmltypes"
            ]
            fileTags: ["vivid-armhf"]
            qbs.install: true
            qbs.installDir: project.pyothersideInstallDir
            qbs.installSourceBase: path + "/prebuilt"
        }
    }
}

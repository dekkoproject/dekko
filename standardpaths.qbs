import qbs

Project {
    name: "Snap Standard Paths"

    property string libDir: "lib"

    DynamicLibrary {
        name: "SnapStandardPaths"
        targetName: "snap-standard-paths"

        Depends { name: "cpp" }
        Depends { name: "Qt.core" }
        cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
        cpp.debugInformation: qbs.buildVariant === "debug"
        cpp.includePaths: [path]
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++"; // avoid issues with libc++
        Export {
            Depends { name: "cpp" }
            Depends { name: "Qt.core" }
            cpp.includePaths: [path]
            cpp.cxxLanguageVersion: "c++11";
            cpp.cxxStandardLibrary: "libstdc++"; // avoid issues with libc++
        }

        Group {
            name: "C++ headers"
            files: "*.h"
        }

        Group {
            name: "C++ sources"
            files: "*.cpp"
        }
        Group {
            qbs.install: true
            qbs.installDir: project.libDir
            fileTagsFilter: product.type
        }
    }
}

import qbs

DynamicLibrary {
    name: "Storage Manager"
    targetName: "qmfstoragemanager"

    Depends { name: "cpp" }
    Depends { name: "QmfClient" }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: [
        path + "/qmfstoragemanager/"
    ]

    Group {
        name: "C++ Sources"
        prefix: path + "/qmfstoragemanager/"
        files: [
            "qmfstoragemanager.h",
            "qmfstoragemanager.cpp"
        ]
    }

    Group {
        qbs.install: true
        qbs.installDir: project.qmfInstallRoot + "/plugins5/contentmanagers"
        fileTagsFilter: product.type
    }
}

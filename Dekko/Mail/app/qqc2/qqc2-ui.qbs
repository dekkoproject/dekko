import qbs

Group {
    condition: project.ui === "qqc2"
    name: "QQC2 UI"
    files: [path + "/qml/**"]
    qbs.install: project.ui === "qqc2"
    qbs.installDir: project.binDir + "/plugins/ui"
    qbs.installSourceBase: path + "/qml"
}

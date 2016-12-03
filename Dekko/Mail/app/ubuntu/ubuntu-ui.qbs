import qbs

Group {
    condition: project.ui === "ubuntu"
    name: "Ubuntu UI"
    files: [path + "/qml/**"]
    qbs.install: project.ui === "ubuntu"
    qbs.installDir: project.binDir + "/plugins/ui/"
    qbs.installSourceBase: path + "/qml"
}

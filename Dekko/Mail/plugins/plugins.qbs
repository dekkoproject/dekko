import qbs

Project {
    name: "Plugins"

    SubProject {
        filePath: "ubuntu/ubuntu-ui.qbs"
        inheritProperties: true
    }
}

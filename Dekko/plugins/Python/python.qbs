import qbs
import qbs.Process

Project {
    name: "Python"

    Product {
        name: project.name + " Plugin"
        type: "python-qml-plugin"

        // This probe does all the hard work of fetching from pypi
        Probe {
            id: pip3
            // Declare your list of pypi packages here
            readonly property string requirementsFile: project.sourceDirectory + "/requirements.txt"
            // DO NOT EDIT MANUALLY. this would normally be hidden
            // inside a module.
            property bool installed: false
            // The directory where you want to install pypi packages
            // You might like to add this directory to you .gitignore/.bzrignore
            property string sourceDir: project.sourceDirectory + "/pylibs"

            configure: {
                if (!requirementsFile.length) {
                    console.info("No python packages to install")
                    return
                }
                // we use the Process service to run pip3
                var p = new Process();
                p.setWorkingDirectory(path)
                // override pip's idea of --user location
                p.setEnv("PYTHONUSERBASE", sourceDir)
                // Finally run pip
                if (p.exec("/usr/bin/pip3", ["install", "-r", requirementsFile, "--user"] , true) === 0) {
                    installed = true
                } else {
                   throw "Pip Not working"
                }
            }
        }

        // Here we define the python directories we want to include in our
        // qml plugin.
        Group {
            // Only enable this if pip3 installed correctly
            condition: pip3.installed
            name: "Python Libs"
            prefix: pip3.sourceDir + "/lib/python3.5/site-packages/"
            // List the package dirs you want in the qml plugin
            // The idea here is to only declare what's _required_ to reduce our size a bit
            files: [
                "markdown/**",
                "jinja2/**",
                "markupsafe/**",
                "bs4/**",
                "cssutils/**",
                "encutils/**",
                "pynliner/**",
                "pygments/**",
                "html2text/**"
            ]
            // cssutils comes with a large test suite
            // which we don't really need in the snap
            // it saves us about 1MB
            excludeFiles: [
                "cssutils/tests/**"
            ]
            fileTags: ["py-libs"]
        }
        // Install directive for the python libs above
        Group {
            fileTagsFilter: "py-libs"
            qbs.install: true
            qbs.installDir: project.qmlDir + "/Dekko/" + project.name
            // This ensures we maintain the directory structure
            qbs.installSourceBase: pip3.sourceDir + "/lib/python3.5/site-packages"
        }

        Group {
            files: [
                "DekkoPython.qml",
            ]
            name: "QML components"
            fileTags: ["py-comps"]
        }

        Group {
            files: [
                "qmldir",
            ]
            name: "QML directory"
            fileTags: ["py-comps"]
        }

        Group {
            fileTagsFilter: "py-comps"
            qbs.install: true
            qbs.installDir: project.qmlDir + "/Dekko/" + project.name
            qbs.installSourceBase: path
        }
    }
}

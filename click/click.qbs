import qbs

Product {
    name: "Click"
    type: "clickpackage"
    builtByDefault: project.click

    Group {
        name: "AppArmor"
        files: [
            "openstore/dekko2.apparmor",
            "openstore/dekkod.apparmor"
        ]
        fileTags: ["clickroot"]
    }

//    Group {
//        name: "Online Accounts"
//        files: [
//            "dekko.dekkoproject_dekko.*"
//        ]
//    }

    Group {
        name: "Click Manifests"
        files: [
            "dekko2-content.json",
            "dekko2.url-dispatcher",
            "manifest.json",
        ]
        fileTags: ["clickroot"]
    }

//    Group {
//        name: "Push Helpers"
//        files: [
//            "dekko-helper"
//        ]
//    }

    Group {
        name: "Launcher"
        files: [
            "dekko-launch",
            "dekkod-launch"
        ]
        fileTags: ["clickbin"]
    }

    Group {
        name: "Desktop Files"
        files: [
            "dekko2.desktop",
            "dekkod.desktop",
        ]
        fileTags: ["clickroot"]
    }

    Group {
        condition: project.click
        fileTagsFilter: "clickroot"
        qbs.install: product.type
        qbs.installDir: "/"
    }

    Group {
        condition: project.click
        fileTagsFilter: "clickbin"
        qbs.install: product.type
        qbs.installDir: project.binDir
    }
}

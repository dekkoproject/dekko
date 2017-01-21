import qbs

Project {
    name: "Plugins"
    
    references: [
        "default/addressbook/addressbook.qbs",
        "default/html-viewer/html-viewer.qbs",
        "Python/python.qbs",
        "Markdown/markdown.qbs"
    ]
}

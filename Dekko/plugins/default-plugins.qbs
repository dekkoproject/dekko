import qbs

Project {
    name: "Plugins"
    
    references: [
        "default/addressbook/addressbook.qbs",
        "Python/python.qbs",
        "Markdown/markdown.qbs"
    ]
}

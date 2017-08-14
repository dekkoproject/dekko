import qbs

Project {
    name: "Extensions"
    
    references: [
        "addressbook/addressbook.qbs",
        "html-viewer/html-viewer.qbs",
        "Python/python.qbs",
        "Markdown/markdown.qbs"
    ]
}

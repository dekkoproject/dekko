import QtQuick 2.4
import io.thp.pyotherside 1.4

Python {
    id: python

    signal ready()

    property var importPaths: []

    Component.onCompleted: {
        addImportPath(Qt.resolvedUrl("./"))
        for (var i in importPaths) {
            addImportPath(Qt.resolvedUrl(importPaths[i]))
        }
        ready()
    }
}

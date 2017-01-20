import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Web 0.2

Page {
    id: viewer
    property string html: ""

    header: PageHeader {
        id: ph
        title: qsTr("HTML Viewer")
    }

    WebView {
        anchors {
            left: parent.left
            top: ph.bottom
            right: parent.right
            bottom: parent.bottom
        }

        Component.onCompleted: {
            console.log("HTML IS: ", html)
            if (viewer.html) {
                loadHtml(html, Qt.resolvedUrl("."))
            }
        }
    }
}

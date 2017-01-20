import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Web 0.2

DekkoPage {
    id: viewer
    property string html: ""

    pageHeader.title: qsTr("HTML Viewer")

    PageContent {
    WebView {
        anchors.fill: parent
        Component.onCompleted: {
            console.log("HTML IS: ", html)
            if (viewer.html) {
                loadHtml(html, Qt.resolvedUrl("."))
            }
        }
    }
    }
}

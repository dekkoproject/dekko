import QtQuick 2.4
import QtQuick.Window 2.1
import Ubuntu.Components 1.3
import MazDB 1.0
import "./views/components"
import Dekko.Markdown 1.0
import QuickFlux 1.0
import Dekko.Mail.API 1.0

Window {
    id: window
    visible: true
    minimumWidth: units.gu(30)
    minimumHeight: units.gu(30)
    width: units.gu(120)
    height: units.gu(80)
    flags: Qt.Window
    modality: Qt.NonModal

    title: qsTr("Dekko Mail")

    MazDBSettings {
        category: "window-property-cache"
        property alias windowWidth: window.width
        property alias windowHeight: window.height
        property alias windowVisible: window.visible
        property alias windowX: window.x
        property alias windowY: window.y
    }

    Item {
        anchors.fill: parent
        Loader {
            asynchronous: false
            anchors.fill: parent
            source: Qt.resolvedUrl("./Dekko.qml")
        }
    }
}

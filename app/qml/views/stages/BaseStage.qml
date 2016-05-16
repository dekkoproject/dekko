import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import "../components"
import "../utils"

StretchColumn {
    anchors {fill: parent}
    default property alias content: mainContainer.data
    property alias panelEnabled: pc.visible
    // We want the main content to take up as much space as possible
    // So use the Stretcher to get this. We are going to add a tabbed
    // panel below this at some point to show log output etc
    Stretcher {
        id: mainContainer
        anchors {
            left: parent.left
            right: parent.right
        }
    }

    PanelContainer {
        id: pc
        width: parent.width
        visible: false
        activeEdge: Item.Top
        DevPanel {
            id: panel
            anchors.fill: parent
        }
    }
}


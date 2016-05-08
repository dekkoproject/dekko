import QtQuick 2.4
import Dekko.Components 1.0
import "../components"

StretchColumn {
    anchors {fill: parent}
    default property alias content: mainContainer.data
    property alias panelContent: panel.data
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
        activeEdge: Item.Top
        Item {
            id: panel
            anchors.fill: parent
        }
    }
}


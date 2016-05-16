import QtQuick 2.4
import Ubuntu.Components 1.3
import "../components"

BottomEdgeConfiguration {
    sourceComponent: DekkoPage {
        width: dekkoPage.width
        height: dekkoPage.height
        Rectangle {
            anchors.fill: parent
            color: "#FFFFFF"
        }
        pageHeader.title: "New message"
        pageHeader.backAction: Action {
            iconName: "down"
            onTriggered: bottomEdge.collapse()
        }

        MessageComposer {
            anchors.fill: parent
        }
    }
}


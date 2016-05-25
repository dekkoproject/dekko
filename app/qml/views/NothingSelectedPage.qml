import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import "./components"
import "../constants"
import "./composer"

DekkoPage {
    id: nowtSelected

    extendHeader: true

    bottomEdgeConfig: BottomEdgeComposer {
        hintVisible: false // we don't need a hint in this view
        enabled: dekko.viewState.isMediumFF && (internalStack.depth === 1)
        canActionTrigger: dekko.viewState.isMediumFF
        listenerEnabled: enabled
    }

    CachedImage {
        id: image
        height: units.gu(20)
        width: height
        anchors.centerIn: parent
        name: Icons.InboxIcon
        color: UbuntuColors.ash
    }

    Label {
        fontSize: "large"
        text: qsTr("No message selected")
        anchors {
            top: image.bottom
            topMargin: Style.defaultSpacing
            horizontalCenter: parent.horizontalCenter
        }
    }
}


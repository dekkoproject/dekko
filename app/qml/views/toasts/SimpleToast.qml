import QtQuick 2.4
import Ubuntu.Components 1.3

UbuntuShape {
    id: rootItem

    property alias text: label.text
    property int displayTime: 1500

    signal closed()

    anchors {
        horizontalCenter: parent.horizontalCenter
        top: parent.top
        topMargin: units.gu(12)
    }
    color: UbuntuColors.inkstone
    aspect: UbuntuShape.Flat
    width: label.width + units.gu(2)
    height: label.height + units.gu(1.5)

    Label {
        id: label
        anchors.centerIn: parent
        color: "#ffffff"
    }

    SequentialAnimation {
        id: showAnimation
        running: true
        NumberAnimation { target: rootItem; property: "opacity"; to: 0.9; duration: 500 }
        PauseAnimation { duration: displayTime }
        ScriptAction { script: destroyAnimation.restart() }
    }

    SequentialAnimation {
        id: destroyAnimation
        NumberAnimation { target: rootItem; property: "opacity"; to: 0; duration: 500 }
        ScriptAction { script: rootItem.closed() }
    }
}


import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0

AbstractButton {
    id: button
    implicitWidth: width
    implicitHeight: height
    width: icon.width + label.width + units.gu(3)
    property alias iconName: icon.name
    property alias iconColor: icon.color

    UbuntuShape {
        anchors {
            topMargin: units.gu(0.5)
            bottomMargin: units.gu(0.5)
            fill: parent
        }
        aspect: UbuntuShape.DropShadow
//        visible: button.pressed
        color: button.pressed ? UbuntuColors.silk : UbuntuColors.porcelain
    }

    CachedImage {
        id: icon
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            margins: units.gu(1)
        }
        height: units.gu(2.25)
        width: height
    }

    Label {
        id: label
        anchors {
            left: icon.right
            leftMargin: units.gu(1)
            verticalCenter: parent.verticalCenter
        }
        text: action.text
    }
}


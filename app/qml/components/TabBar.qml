import QtQuick 2.4
import Ubuntu.Components 1.3

Item {
    id: tabBar
    property int currentIndex: 0
    property alias iconNameModel: rp.model
    height: units.gu(6)

    Row {
        id: actionRow
        width: parent.width
        height: parent.height

        Repeater {
            id: rp
            delegate: Rectangle {
                id: btn
                width: tabBar.width / 4
                height: parent.height
                AbstractButton {
                    anchors.fill: parent
                    onPressedChanged: pressed ? btn.color = UbuntuColors.lightGrey : btn.color = "transparent"
                    onClicked: tabBar.currentIndex = model.index
                    Icon {
                        height: units.gu(2.5); width: height
                        name: modelData
                        anchors.centerIn: parent
                        color: tabBar.currentIndex === model.index ? UbuntuColors.blue : UbuntuColors.ash
                    }
                }
                Rectangle {
                    anchors {
                        left: parent.left
                        bottom: parent.bottom
                        right: parent.right
                    }
                    height: units.dp(2)
                    color: tabBar.currentIndex === model.index ? UbuntuColors.blue : UbuntuColors.ash
                }
            }
        }
    }
    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            top: tabBar.top
        }
        height: units.dp(1)
        color: UbuntuColors.lightGrey
    }
}


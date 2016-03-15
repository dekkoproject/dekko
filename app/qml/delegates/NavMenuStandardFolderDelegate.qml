import QtQuick 2.4
import Ubuntu.Components 1.3

ListItem {
    id: li

    property var folder

    expansion.expanded: false
    expansion.height: layout.height + col.height
    height: layout.implicitHeight
    ListItemLayout {
        id: layout
        title.text: folder.displayName
        height: units.gu(7)

        Icon {
            height: units.gu(3)
            width: height
            name: "inbox"
            SlotsLayout.position: SlotsLayout.Leading
        }

        UbuntuShape {
            id: shape
            visible: folder.unreadCount > 0
            aspect: UbuntuShape.Flat
            color: UbuntuColors.green
            height: units.gu(2.2)
            width: countLable.width < height ? height : countLable.width + units.gu(1)
            Label {
                id: countLable
                anchors.margins: units.gu(0.5)
                anchors.centerIn: parent
                fontSize: "small"
                color: "#ffffff"
                text: folder.unreadCount
            }
        }

        Item {
//            visible: folder.hasDescendents
            visible: model.index === 0
            height: units.gu(3)
            width: units.gu(4)
            Rectangle {
                anchors.left: parent.left
                height: parent.height
                width: units.dp(1)
                color: UbuntuColors.lightGrey
            }
            Icon {
                height: units.gu(2)
                width: height
                anchors.right: parent.right
                anchors.rightMargin: units.gu(0.5)
                anchors.verticalCenter: parent.verticalCenter
                name: li.expansion.expanded ? "up" : "down"
            }
            SlotsLayout.position: SlotsLayout.Last
            MouseArea {
                anchors.fill: parent
                onClicked: li.expansion.expanded = !li.expansion.expanded
            }
        }
    }
    Column {
        id: col
        visible: folder.hasDescendents
        anchors.left: parent.left
        anchors.leftMargin: units.gu(2)
        anchors.top: layout.bottom
        anchors.right: parent.right
        Repeater {
            model: folder.descendents
            delegate: ListItem {
                id: delegate
                divider.visible: false
                height: dlayout.height
                ListItemLayout {
                    id: dlayout
                    height: units.gu(5)
                    title.text: qtObject.displayName
                    Icon {
                        height: units.gu(2.5)
                        width: height
                        name: "inbox"
                        SlotsLayout.position: SlotsLayout.Leading
                    }

                    UbuntuShape {
                        id: dshape
                        visible: qtObject.unreadCount > 0
                        aspect: UbuntuShape.Flat
                        color: UbuntuColors.green
                        height: units.gu(2.2)
                        width: dcountLable.width < height ? height : dcountLable.width + units.gu(1)
                        Label {
                            id: dcountLable
                            anchors.margins: units.gu(0.5)
                            anchors.centerIn: parent
                            color: "#ffffff"
                            text: qtObject.unreadCount
                            fontSize: "small"
                        }
                    }
                }
            }
        }
    }
}


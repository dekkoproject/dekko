import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Dekko.Mail 1.0
import "../components"

ListItem {
    property var msg
    property alias model: repeater.model
    property alias title: tl.text
    property int minHeight: units.gu(5)
    property int internalHeight: c.height + units.gu(3)
    height: internalHeight < minHeight ? minHeight : internalHeight
    Item {
        id: p
        width: tl.width + units.gu(1)
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        Label {
            id: tl
            anchors {
                left: parent.left
                top: parent.top
                topMargin: units.gu(2)
                leftMargin: units.gu(1)
            }
        }
    }

    Flow {
        id: c
        anchors {
            left: p.right
            leftMargin: units.gu(1)
            top: parent.top
            topMargin: units.gu(1.5)
            right: parent.right
            rightMargin: units.gu(1)
        }
        spacing: units.gu(1)

        Repeater {
            id: repeater
            delegate: UbuntuShape {
                id: delegate
                aspect: UbuntuShape.Flat
                color: Qt.rgba(0, 0, 0, 0.05)
                radius: "small"
                height: units.gu(3)
                width: inner_avatar.width + label.width + units.gu(1.5)

                Avatar {
                    id: inner_avatar
                    width: height
                    anchors {
                        left: parent.left
                        top: parent.top
                        bottom: parent.bottom
                        margins: units.dp(1)
                    }
                    name: model.qtObject.name
                    initials: model.qtObject.initials
                    email: model.qtObject.address
                    fontSize: "x-small"
                    validContact: true
                }

                Label {
                    id: label
                    anchors {
                        left: inner_avatar.right
                        leftMargin: units.gu(0.5)
                        verticalCenter: parent.verticalCenter
                    }
                    text: model.qtObject.name
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        PopupUtils.open("qrc:/qml/views/popovers/RecipientPopover.qml", delegate, {address: model.qtObject})
                    }
                }
            }
        }
    }
}


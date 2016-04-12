import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Dekko.Components 1.0
import "../../components"
import "../../utils/UiUtils.js" as UiUtils

ListItem {
    property var msg
    width: parent.width
    height: col.height + units.gu(1)
    divider.visible: false

    Column {
        id: col
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        spacing: units.gu(0.5)

        Label {
            anchors {
                left: parent.left
                right: parent.right
            }
            text: msg.prettyLongDate
            horizontalAlignment: Text.AlignHCenter
            fontSize: "small"
        }
        Item {
            anchors {
                left: parent.left
                right: parent.right
            }
            height: s.height
            Label {
                id: s
                width: parent.width - units.gu(6)
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                }
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: msg.subject
                horizontalAlignment: Text.AlignHCenter
                fontSize: "medium"
                font.weight: Font.DemiBold
            }
        }
        Item {
            anchors {
                left: parent.left
                right: parent.right
            }
            height: r.height
            Item {
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                }
                width: r.width + d.width + div.width + units.gu(2)
                Label {
                    id: r
                    anchors {
                        left: parent.left
                        top: parent.top
                    }
                    color: UbuntuColors.blue
                    text: msg.from.name
                    fontSize: "small"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: PopupUtils.open("qrc:/qml/popovers/RecipientPopover.qml", r, {address: msg.from})
                    }
                }
                Label {
                    id: div
                    text: "|"
                    anchors {
                        left: r.right
                        leftMargin: units.gu(1)
                        top: parent.top
                    }
                    fontSize: "small"
                }
                Label {
                    id: d
                    anchors {
                        top: parent.top
                        left: div.right
                        leftMargin: units.gu(1)
                    }
                    color: UbuntuColors.blue
                    text: qsTr("Details")
                    fontSize: "small"
                    // :/qml/popovers/MessageDetailsPopover.qml
                    MouseArea {
                        anchors.fill: parent
                        onClicked: PopupUtils.open("qrc:/qml/popovers/MessageDetailsPopover.qml", d, {message: msg})
                    }
                }
            }
        }
    }
}


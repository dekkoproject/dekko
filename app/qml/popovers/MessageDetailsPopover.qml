import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Dekko.Mail 1.0
import Dekko.Controls 1.0
import "../components"
import "../views/messageview"

Popover {
    id: info
    property var message

//    contentWidth: units.gu(35)
    contentHeight: units.gu(30)
    Rectangle {
        anchors.fill: parent
        color: UbuntuColors.porcelain
    }

    Column {
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }

        Item {
            id: internal
            width: parent.width
            height: info.height

            StackView {
                id: detailStack
                anchors.fill: parent// make sure our stack takes the full size
                initialItem: DetailList {
                    property int currentHeight: internalHeight
                    width: detailStack.width
                    height: detailStack.height
                    message: info.message
                }
            }
        }
    }
}


import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Dekko.Mail 1.0
import "../components"
import "../delegates"

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
            delegate: RecipientDelegate {
                id: delegate
            }
        }
    }
}


import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Controls 1.0
import Dekko.Mail 1.0
import "../components"

DetailItem {
    id: dl
    property var message
    internalHeight: col.height > units.gu(25) ? col.height : units.gu(25)
    height: parent.height
    width: parent.width
    color: UbuntuColors.porcelain
    ScrollView {
        anchors.fill: parent
        Flickable {
            anchors.fill: parent
            contentHeight: col.height
            Column {
                id: col
                anchors {
                    left: parent.left
                    top: parent.top
                    right: parent.right
                }
                ListItem {
                    height: units.gu(4)
                    Label {
                        anchors.centerIn: parent
                        text: qsTr("Details")
                        font.weight: Font.DemiBold
                    }
                }
                RecipientFlow {
                    title: qsTr("To:")
                    message: dl.message
                    model: message.to
                }
                RecipientFlow {
                    visible: message.cc.count
                    title: qsTr("Cc:")
                    message: dl.message
                    model: message.cc
                }
            }
        }
    }
}


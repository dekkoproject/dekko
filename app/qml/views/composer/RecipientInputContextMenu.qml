import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Dekko.Mail 1.0
import "../../actions/composer"
import "../../stores/composer"

Popover {
    id: info
    property int recipientType: RecipientType.To

    contentHeight: col.height
    contentWidth: units.gu(20)

    QtObject {
        id: d
        readonly property bool isToField: recipientType === RecipientType.To
    }

    UbuntuShape {
        aspect: UbuntuShape.DropShadow
        color: UbuntuColors.porcelain
        width: units.gu(20)
        height: col.height
        Column {
            id: col
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            ListItem {
                visible: !ComposerStore.showCC && d.isToField
                height: ccl.height
                ListItemLayout {
                    id: ccl
                    height: units.gu(6)
                    title.text: qsTr("Add CC")
                }
                onClicked: {
                    ComposerActions.addCC()
                    PopupUtils.close(info)
                }
            }
            ListItem {
                visible: !ComposerStore.showBCC && d.isToField
                height: bccl.height
                ListItemLayout {
                    id: bccl
                    height: units.gu(6)
                    title.text: qsTr("Add BCC")
                }
                onClicked: {
                    ComposerActions.addBCC()
                    PopupUtils.close(info)
                }
            }
            ListItem {
                height: acl.height
                ListItemLayout {
                    id: acl
                    height: units.gu(6)
                    title.text: qsTr("Add contact")
                }
                onClicked: {
                    ComposerActions.addRecipientFromContacts(recipientType)
                    PopupUtils.close(info)
                }
            }
        }
    }
}


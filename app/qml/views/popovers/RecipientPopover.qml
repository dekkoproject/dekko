import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Dekko.Mail 1.0
import "../components"

Popover {
    id: info
    property var address

    Column {
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }

        ListItem {
            height: layout.height
            color: UbuntuColors.porcelain
            ListItemLayout {
                id: layout
                title.text: address ? address.name : model.email
                title.elide: Text.ElideRight
                title.wrapMode: Text.NoWrap
                subtitle.text: {
                    if (address.name) {
                        if (address.name !== address.address)
                            return address.address
                        else
                            return ""
                    }/* else if (model.isInContacts) {
                        return model.contactInfo.organization
                    } else {
                        return ""
                    }*/
                }
//                summary.text: address.name && model.isInContacts ? model.contactInfo.organization : ""
                subtitle.elide: Text.ElideRight
                subtitle.wrapMode: Text.NoWrap

                Avatar {
                    id: avatar
                    name: address.name
                    initials: address.initials
                    email: address.address
                    validContact: true
                    fontSize: "large"
                    SlotsLayout.position: SlotsLayout.Leading
                }
            }
        }

//        ListItem {
//            height: deleteLayout.implicitHeight
//            ListItemLayout {
//                id: deleteLayout
//                title.text: qsTr("Remove")
//                title.font.weight: Style.common.fontWeight
//                title.elide: Text.ElideRight
//                title.wrapMode: Text.NoWrap
//            }
//            onClicked: info.remove()
//        }
        ListItem {
            height: clipboard.implicitHeight
            color: UbuntuColors.porcelain
            ListItemLayout {
                id: clipboard
                title.text: qsTr("Copy to clipboard")
                title.elide: Text.ElideRight
                title.wrapMode: Text.NoWrap
            }
            onClicked: {
                dekko.showNotice("Not implemented yet. Fix it before release!!!!")
                PopupUtils.close(info)
            }
        }
        ListItem {
            height: addContactLayout.implicitHeight
            ListItemLayout {
                id: addContactLayout
                title.text: qsTr("Add to addressbook")
                title.elide: Text.ElideRight
                title.wrapMode: Text.NoWrap
            }
            color: UbuntuColors.porcelain
            onClicked: {
                dekko.showNotice("Not implemented yet. Fix it before release!!!!")
                PopupUtils.close(info)
            }
        }
        ListItem {
            height: send.implicitHeight
            color: UbuntuColors.porcelain
            ListItemLayout {
                id: send
                title.text: qsTr("Send message")
                title.elide: Text.ElideRight
                title.wrapMode: Text.NoWrap
            }
            onClicked: {
                dekko.showNotice("Not implemented yet. Fix it before release!!!!")
                PopupUtils.close(info)
            }
        }
    }
}


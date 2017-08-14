/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu devices

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Dekko.Components 1.0
import Dekko.Ubuntu.Components 1.0

DetailItem {
    property var address
    internalHeight: col.height
    height: parent.height
    width: parent.width
    color: UbuntuColors.porcelain
    Column {
        id: col
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        ListItem {
            height: l.height
            ListItemLayout {
                id: l
                title.text: qsTr("Back")
                Icon {
                    name: "back"
                    height: units.gu(2.5); width: height
                    SlotsLayout.position: SlotsLayout.Leading
                }
            }
            onClicked: detailStack.pop()
        }

        ListItem {
            height: layout.height
            PixelPerfectItem {
                anchors.fill: parent
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
        }
        ListItem {
            height: clipboard.height
            ListItemLayout {
                id: clipboard
                height: units.gu(5)
                title.text: qsTr("Copy to clipboard")
                title.elide: Text.ElideRight
                title.wrapMode: Text.NoWrap
            }
            onClicked: {
                PopupActions.showNotice("Not implemented yet. Fix it before release!!!!")
                PopupUtils.close(info)
            }
        }
        ListItem {
            height: addContactLayout.height
            ListItemLayout {
                id: addContactLayout
                height: units.gu(5)
                title.text: qsTr("Add to addressbook")
                title.elide: Text.ElideRight
                title.wrapMode: Text.NoWrap
            }
            onClicked: {
                PopupActions.showNotice("Not implemented yet. Fix it before release!!!!")
                PopupUtils.close(info)
            }
        }

        ListItem {
            height: send.height
            ListItemLayout {
                id: send
                height: units.gu(5)
                title.text: qsTr("Send message")
                title.elide: Text.ElideRight
                title.wrapMode: Text.NoWrap
            }
            onClicked: {
                PopupActions.showNotice("Not implemented yet. Fix it before release!!!!")
                PopupUtils.close(info)
            }
        }
    }
}

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
import Dekko.Mail 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Composer 1.0

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


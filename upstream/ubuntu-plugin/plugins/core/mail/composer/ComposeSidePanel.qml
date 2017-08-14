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
import Dekko.Components 1.0
import QuickFlux 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Composer 1.0
//import Dekko.Contacts.Stores 1.0
import Dekko.Ubuntu.Constants 1.0
import Dekko.Ubuntu.Components 1.0
import "../contacts"

PanelContainer {
    id: sp
    visible: width > 0
    resizable: !dekko.isSmallFF
    minSize: units.gu(20)
    maxSize: units.gu(30)
    size: ComposerStore.sidePanelOpen ? units.gu(25) : 0
    height: parent.height
    activeEdge: Item.Left

    Item {
        id: bar
        property int currentIndex: 0
        height: units.gu(4)
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        AbstractButton {
            id: attachButton
            height: parent.height
            width: parent.width / 2
            anchors {
                left: parent.left
                top: parent.top
            }
            Rectangle {
                anchors.fill: parent
                visible: attachButton.pressed /*|| bar.currentIndex === 0*/
                color: UbuntuColors.silk
            }
            onClicked: bar.currentIndex = 0
            CachedImage {
                anchors.centerIn: parent
                name: Icons.AttachmentIcon
                height: units.gu(3)
                width: height
                color: bar.currentIndex === 0 ? UbuntuColors.blue : UbuntuColors.ash
            }
            Rectangle {
                anchors {
                    left: parent.left
                    bottom: parent.bottom
                    right: parent.right
                }
                height: units.dp(2)
                color: UbuntuColors.blue
                visible: bar.currentIndex === 0
            }
            Line {
                anchors {
                    right: parent.right
                    top: parent.top
                    topMargin: units.gu(0.5)
                    bottom: parent.bottom
                    bottomMargin: units.gu(0.5)
                }
            }
        }

        AbstractButton {
            id: contactButton
            height: parent.height
            width: parent.width / 2
            anchors {
                right: parent.right
                top: parent.top
            }
            Rectangle {
                anchors.fill: parent
                visible: contactButton.pressed /*|| bar.currentIndex === 1*/
                color: UbuntuColors.silk
            }
            onClicked: bar.currentIndex = 1
            CachedImage {
                anchors.centerIn: parent
                name: Icons.NewContactIcon
                height: units.gu(3)
                width: height
                color: bar.currentIndex === 1 ? UbuntuColors.blue : UbuntuColors.ash
            }
            Rectangle {
                anchors {
                    left: parent.left
                    bottom: parent.bottom
                    right: parent.right
                }
                height: units.dp(2)
                color: UbuntuColors.blue
                visible: bar.currentIndex === 1
            }
        }

        Line {
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
        }
    }

    AttachmentList {
        visible: bar.currentIndex === 0
        anchors {
            top: parent.top
            topMargin: units.gu(4)
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }

//    ContactsListView {
//        visible: bar.currentIndex === 1
//        addressBook: ContactsStore.selectedAddressBook
//        filterEmailOnly: true
//        state: "slimPicker"
//        anchors {
//            top: parent.top
//            topMargin: units.gu(4)
//            left: parent.left
//            right: parent.right
//            bottom: parent.bottom
//        }
//    }
}

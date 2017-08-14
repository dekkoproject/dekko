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
import Dekko.Controls 1.0
import Dekko.Mail 1.0
import Dekko.Ubuntu.Components 1.0

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


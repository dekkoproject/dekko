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
import Dekko.Ubuntu.Components 1.0
import Dekko.Ubuntu.Constants 1.0
import "../composer"

DekkoPage {
    id: nowtSelected

    extendHeader: true


    PageContent {

        Item {
            height: image.height + label.height + Style.defaultSpacing
            width: parent.width
            anchors {
                verticalCenter: parent.verticalCenter
                verticalCenterOffset: -units.gu(5)
            }
            CachedImage {
                id: image
                height: units.gu(20)
                width: height
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                }
                name: Icons.InboxIcon
                color: UbuntuColors.ash
            }

            Label {
                id: label
                fontSize: "large"
                text: qsTr("No message selected")
                anchors {
                    top: image.bottom
                    topMargin: Style.defaultSpacing
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }
}


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
import Dekko.Controls 1.0
import Dekko.Ubuntu.Components 1.0
import "../messageview"

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


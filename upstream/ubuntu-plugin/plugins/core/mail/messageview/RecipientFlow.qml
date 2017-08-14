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
import Dekko.Ubuntu.Components 1.0
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


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
import Dekko.Ubuntu.Constants 1.0

Item {

    property alias text: label.text
    property int textMargin: Style.defaultSpacing
    property alias backgroundEnabled: background.visible
    property alias backgroundColor: background.color
    property alias dividerVisible: divider.visible

    height: units.gu(3)
    anchors {
        left: parent.left
        right: parent.right
    }

    Rectangle {
        id: background
        visible: false
        anchors.fill: parent
        color: UbuntuColors.porcelain
    }

    Label {
        id: label
        anchors {
            left: parent.left
            leftMargin: textMargin
            verticalCenter: parent.verticalCenter
        }
        font.weight: Font.DemiBold
        color: UbuntuColors.inkstone
    }

    Line {
        id: divider
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
    }
}

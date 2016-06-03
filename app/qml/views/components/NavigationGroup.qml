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

ListItem {
    id: group
    property alias title: sLabel.text
    height: titleLabel.height
    divider.visible: false
    expansion.height: titleLabel.height + content.height
    property alias model: repeater.model
    property alias delegate: repeater.delegate
    signal expandClicked()
    ListItem {
        id: titleLabel
        anchors {
            left: parent.left
            right: parent.right
        }
        visible: model.count
        height: sLabel.height
        Label {
            id: sLabel
            anchors {
                left: parent.left
                leftMargin: units.gu(2)
                right: parent.right
                rightMargin: units.gu(2)
            }
            text: group.title
            height: units.gu(4)
            fontSize: "medium"
            font.weight: Font.DemiBold
            verticalAlignment: Text.AlignVCenter

        }
        Icon {
            height: units.gu(3)
            width: height
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
                rightMargin: units.gu(1.5)
            }
            name: group.expansion.expanded ? "view-collapse" : "view-expand"
        }
        onClicked: {
            group.expandClicked()
        }
    }

    Column {
        id: content
        anchors {
            left: parent.left
            top: titleLabel.bottom
            right: parent.right
        }

        Repeater {
            id: repeater
        }
    }
}


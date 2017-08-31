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
    id: root
    clip: true
    width: parent.width
    height: btn.height
    property alias title: lbl.text
    property alias model: repeater.model
    property alias delegate: repeater.delegate

    property bool expanded: false

    AbstractButton {
        id: btn
        height: lbl.height + units.gu(2)
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }

        onClicked: root.expanded = !root.expanded

        Rectangle {
            anchors.fill: parent
            color: UbuntuColors.slate
            visible: btn.pressed
        }

        Label {
            id: lbl
            anchors {
                left: parent.left
                leftMargin: units.gu(1)
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            color: UbuntuColors.ash
        }

        Icon {
            height: units.gu(3)
            width: height
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
                rightMargin: units.gu(1.5)
            }
            name: root.expanded ? "view-collapse" : "view-expand"
        }
    }

    Column {
        id: col
        anchors {
            left: parent.left
            right: parent.right
            top: btn.bottom
        }

        Repeater {
            id: repeater
        }
    }

    Behavior on height {
        UbuntuNumberAnimation{}
    }

    states: [
        State {
            name: "collapsed"
            when: !root.expanded
            PropertyChanges {
                target: root
                height: btn.height
            }
        },
        State {
            name: "expanded"
            when: root.expanded
            PropertyChanges {
                target: root
                height: btn.height + col.height
            }
        }
    ]
}

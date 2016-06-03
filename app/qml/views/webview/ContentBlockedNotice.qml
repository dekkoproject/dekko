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

Item {
    id: notice

    property bool contentBlocked: false
    readonly property bool isExpanded: state === "expanded"

    signal allowClicked()

    ListItem {
        anchors.fill: parent
        divider.visible: notice.contentBlocked
        color: UbuntuColors.porcelain
        Label {
            anchors {
                left: parent.left
                leftMargin: units.gu(2)
                verticalCenter: parent.verticalCenter
            }
            visible: notice.contentBlocked
            text: qsTr("Remote content blocked")
        }

        Button {
            height: units.gu(4)
            width: units.gu(8)
            anchors {
                right: parent.right
                rightMargin: units.gu(2)
                verticalCenter: parent.verticalCenter
            }
            color: UbuntuColors.green
            action: Action {
                text: qsTr("Allow")
                onTriggered: notice.allowClicked()
            }
        }
    }

    state: contentBlocked ? "expanded" : "collapsed"
    states: [
        State {
            name: "collapsed"
            PropertyChanges {
                target: notice
                height: 0
            }
        },
        State {
            name: "expanded"
            PropertyChanges {
                target: notice
                height: units.gu(6)
            }
        }
    ]
    // We only want to animate on closing. Looks a bit weird
    // on expanding while the view is still being constructed.
    transitions: [
        Transition {
            from: "expanded"
            to: "collapsed"
            animations: [
                NumberAnimation {
                    target: notice
                    property: "height"
                    duration: UbuntuAnimation.FastDuration
                }
            ]
        }
    ]
}


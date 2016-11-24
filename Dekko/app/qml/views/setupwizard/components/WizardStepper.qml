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
    height: units.gu(5)
    property Action previousAction
    property Action nextAction

    anchors {
        left: parent.left
        right: parent.right
    }
    AbstractButton {
        objectName: "wizardStepBack"
        height: parent.height
        width: backLabel.width + units.gu(4)
        anchors.left: parent.left
        Rectangle {
            anchors.fill: parent
            color: UbuntuColors.silk
            visible: parent.pressed
        }
        Icon {
            id: bkIcon
            height: units.gu(1.5)
            width: height
            name: "back"
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
        }

        Label {
            id: backLabel
            text: previousAction.text
            anchors {
                left: bkIcon.right
                leftMargin: units.gu(0.5)
                verticalCenter: parent.verticalCenter
            }
        }
        onClicked: previousAction.trigger()
    }

    AbstractButton {
        objectName: "wizardStepNext"
        height: parent.height
        width: nxtLabel.width + units.gu(4)
        anchors.right: parent.right
        Rectangle {
            anchors.fill: parent
            color: UbuntuColors.silk
            visible: parent.pressed
        }
        Icon {
            id: nxtIcon
            height: units.gu(1.5)
            width: height
            name: "next"
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
        }

        Label {
            id: nxtLabel
            text: nextAction.text
            anchors {
                right: nxtIcon.left
                rightMargin: units.gu(0.5)
                verticalCenter: parent.verticalCenter
            }
        }
        onClicked: nextAction.trigger()
    }
}


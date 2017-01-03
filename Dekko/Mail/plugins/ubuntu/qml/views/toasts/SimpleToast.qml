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
import "../../constants"

UbuntuShape {
    id: rootItem

    property alias text: label.text
    property int displayTime: 1500

    signal closed()

    anchors {
        horizontalCenter: parent.horizontalCenter
        top: parent.top
        topMargin: units.gu(12)
    }
    color: UbuntuColors.inkstone
    aspect: UbuntuShape.Flat
    width: label.width + Style.defaultSpacing
    height: label.height + units.gu(1.5)

    Label {
        id: label
        anchors.centerIn: parent
        color: "#ffffff"
    }

    SequentialAnimation {
        id: showAnimation
        running: true
        NumberAnimation { target: rootItem; property: "opacity"; to: 0.9; duration: 500 }
        PauseAnimation { duration: displayTime }
        ScriptAction { script: destroyAnimation.restart() }
    }

    SequentialAnimation {
        id: destroyAnimation
        NumberAnimation { target: rootItem; property: "opacity"; to: 0; duration: 500 }
        ScriptAction { script: rootItem.closed() }
    }
}


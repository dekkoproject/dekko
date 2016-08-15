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
import Dekko.Components 1.0
import "../../constants"
import "../components"

PopupBase {
    id: base

    property alias title: header.title
    default property alias settingsContent: mainContent.data
    readonly property int maxHeight: units.gu(70)
    readonly property int maxWidth: units.gu(100)

    Rectangle {
        anchors.fill: parent
        color: UbuntuColors.graphite
        opacity: 0.6
    }

    UbuntuShape {
        readonly property bool shouldCenter: height === base.maxHeight
        readonly property bool addTopMargin: height > units.gu(40)
        aspect: UbuntuShape.DropShadow
        width: Math.min((parent.width / 3)*2, base.maxWidth)
        height: Math.min((parent.height / 5)*4, base.maxHeight)
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: shouldCenter ? undefined : parent.top
            topMargin: shouldCenter ? undefined : (addTopMargin ? units.gu(6) : Style.defaultSpacing)
            verticalCenter: shouldCenter ? parent.verticalCenter : undefined
        }
        color: UbuntuColors.porcelain
        clip: true

        StretchColumn {
            anchors.fill: parent

            DekkoHeader {
                id: header
                implicitHeight: height
                backgroundVisible: false
                backAction: Action {
                    iconName: "close"
                    onTriggered: PopupUtils.close(base)
                }
            }

            Stretcher {
                id: mainContent
            }
        }
        InverseMouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            onPressed: PopupUtils.close(base)
        }
    }

}

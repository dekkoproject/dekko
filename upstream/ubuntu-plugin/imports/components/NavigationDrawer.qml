/* Copyright (C) 2014-2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu Devices/

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
import QuickFlux 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.0 as ListItem
import Dekko.Mail.API 1.0

Panel {
    id: accountsDrawer
    anchors {
        left: parent.left
        top: parent.top
        bottom: parent.bottom
    }
    animate: true
    width: units.gu(25)
    property bool enabled: false
    default property alias data: content.data

    property Action action: Action {
        id: drawerAction
        iconName: "navigation-menu"
        onTriggered: accountsDrawer.opened ? accountsDrawer.close() : accountsDrawer.open()
    }

    property int maxHeight

    function delayClose() {
        delayCloseTimer.start()
    }

    AppListener {
        Filter {
            type: ViewKeys.toggleNavDrawer
            onDispatched: accountsDrawer.opened ? accountsDrawer.close() : accountsDrawer.open()
        }
        Filter {
            type: MessageKeys.openFolder
            onDispatched: {
                if (accountsDrawer.opened) {
                    accountsDrawer.delayClose()
                }
            }
        }

        Filter {
            type: ViewKeys.closeNavDrawer
            onDispatched: {
                if (accountsDrawer.opened) {
                    delayClose()
                }
            }
        }
    }

    // Don't let the event propogate to the page below
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: mouse.accepted = true
        onWheel: wheel.accepted = true
    }

    Timer {
        id: delayCloseTimer
        interval: 100
        repeat: false
        onTriggered: accountsDrawer.close()
    }

    visible: opened || animating
    align: Qt.AlignLeading

    InverseMouseArea {
        visible: accountsDrawer.opened
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        onPressed: accountsDrawer.close()
    }
    // FIXME: where this meets the btmShadow there is a clear difference
    // in the way it looks where they join. How to wrap a gradient round a corner??
    // NOTE it's only really visible to the eye if you are looking for it
    HorizontalGradiant {
        id: sideShadow
        anchors {
            left: parent.right
            bottom: parent.bottom
            top: parent.top
        }
        width: units.gu(0.8)
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0, 0.1) }
            GradientStop { position: 1.0; color: "transparent" }
        }
    }
    Item {
        id: content
        anchors.fill: parent
    }
}

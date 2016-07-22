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
import Dekko.Components 1.0
import "../components"
import "../utils"

StretchColumn {
    anchors {fill: parent}
    default property alias content: mainContainer.data
    property alias panelEnabled: pc.visible
    // We want the main content to take up as much space as possible
    // So use the Stretcher to get this. We are going to add a tabbed
    // panel below this at some point to show log output etc
    Stretcher {
        id: mainContainer
        anchors {
            left: parent.left
            right: parent.right
        }
    }

    PanelContainer {
        id: pc
        width: parent.width
        visible: false
        activeEdge: Item.Top
        Loader {
            anchors.fill: parent
            active: pc.visible
            asynchronous: true
            sourceComponent: Component {
                DevPanel {
                    id: panel
                    anchors.fill: parent
                }
            }
        }
    }
}


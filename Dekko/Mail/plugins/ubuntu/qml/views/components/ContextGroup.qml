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
import "../delegates"

ListItem {
    height: group.height
    // Takes list of ContextAction
    property list<ContextAction> contextActions
    PixelPerfectItem {
        anchors.fill: parent

        Column {
            id: group
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }

            Repeater {
                id: repeater
                model: contextActions
                delegate: ContextDelegate {
                    visible: model.visible
                    description: model.description
                    actionIcon: model.actionIcon
                    actionIconColor: model.actionIconColor

                    selectedIcon: model.selectedIcon
                    selected: model.selected
                    selectedIconColor: model.selectedIconColor
                    onClicked: contextActions[model.index].trigger()
                }
            }
        }
    }
}

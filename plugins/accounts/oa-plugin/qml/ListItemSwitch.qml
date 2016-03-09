/* Copyright (C) 2015 Dan Chapman <dpniel@ubuntu.com>

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
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3

ListItem {
    property alias text: label.text
    property alias checked: layoutSwitch.checked
    onClicked: layoutSwitch.checked = !layoutSwitch.checked
    divider.visible: false
    height: units.gu(5)
    RowLayout {
        anchors.fill: parent
        Label {
            id: label
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
        }

        Switch {
            id: layoutSwitch
            Layout.alignment: Qt.AlignVCenter
        }
    }
}


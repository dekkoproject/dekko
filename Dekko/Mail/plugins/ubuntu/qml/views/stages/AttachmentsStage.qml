/* Copyright (C) 2017 Dan Chapman <dpniel@ubuntu.com>

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
import PlugMan 1.0

BaseStage {
    id: attachmentsStage

    ItemRegistry {
        target: attachmentsStage
        location: "Dekko::Stage::Notes"
        loadMode: ItemRegistry.LoadFirstEnabled
        asynchronous: true
        defaultItems: [
            Item {
                anchors.fill: parent
                Text {
                    anchors.centerIn: parent
                    text: qsTr("Coming soon")
                }
            }
        ]
    }
}

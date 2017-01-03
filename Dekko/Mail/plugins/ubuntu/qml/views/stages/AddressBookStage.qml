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
import Dekko.Contacts 1.0
import "../components"
import "../contacts"
import "../../actions/logging"
import "../../actions/views"
import "../../constants"

BaseStage {
    id: ms
    panelEnabled: devModeEnabled

    StretchRow {
        spacing: 0
        anchors.fill: parent
        // Should only be visible on large FF
        // Access is done via the navigation drawer
        // for smaller FF's
        PanelContainer {
            visible: dekko.isLargeFF
            resizable: !dekko.isSmallFF
            minSize: units.gu(30)
            maxSize: units.gu(50)
            height: parent.height
            activeEdge: Item.Right
            StageArea {
                id: navMenuStage
                stageID: ContactKeys.addressBookListStageArea
                anchors.fill: parent
                baseUrl: "qrc:/qml/views/contacts/AddressBookList.qml"
            }
        }

        PanelContainer {
            stretchOnSmallFF: true
            resizable: !dekko.isSmallFF
            minSize: units.gu(40)
            maxSize: units.gu(60)
            size: units.gu(40)
            height: parent.height
            activeEdge: Item.Right
            StageArea {
                id: msgListStage
                stageID: ContactKeys.addressBookStageArea
                anchors.fill: parent
                baseUrl: "qrc:/qml/views/contacts/ContactListPage.qml"
                function rewind() {
                    var needsDelay = false
                    if (stackCount > 1) {
                        needsDelay = true
                        while (stackCount !== 1) {
                            pop()
                        }
                    }
                }
            }
        }
        Stretcher {
            visible: !dekko.isSmallFF
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            StageArea {
                id: msgViewStage
                stageID: ContactKeys.contactViewStageArea
                anchors.fill: parent
                immediatePush: false
                baseUrl: ""
                delegate: DekkoAnimation.customStackViewDelegate1
            }
        }
    }
}


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
import QuickFlux 1.0
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Settings 1.0
import "../components"
import "../settings"
import "../../constants"
import "../utils"

BaseStage {
    id: settingsStage
    property alias viewState: vs

    ViewState {
        id: vs
        anchors.fill: parent
        Component.onCompleted: SettingsStore.viewState = vs
    }

    StretchRow {
        spacing: 0
        anchors.fill: parent
        // Should only be visible on large FF
        // Access is done via the navigation drawer
        // for smaller FF's
        PanelContainer {
            stretchOnSmallFF: true
            viewState: vs
            resizable: !vs.isSmallFF
            minSize: units.gu(30)
            maxSize: units.gu(50)
            size: units.gu(40)
            height: parent.height
            activeEdge: Item.Right
            StageArea {
                id: navMenuStage
                stageID: ViewKeys.settingsStack1
                anchors.fill: parent
                baseUrl: Qt.resolvedUrl("../settings/Settings.qml")
            }
        }
        // Take rest of space when visible
        Stretcher {
            visible: !vs.isSmallFF
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            StageArea {
                id: msgViewStage
                stageID: ViewKeys.settingsStack2
                anchors.fill: parent
                immediatePush: false
                pushBeforeReplace: true
                baseComponent: Component {
                    Item {
                        height: parent.height
                        width: parent.width
                    }
                }
                delegate: DekkoAnimation.customStackViewDelegate1
            }
        }
    }
}


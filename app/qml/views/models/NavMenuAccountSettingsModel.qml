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
import QtQuick.Window 2.2
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import "../../actions/views"

VisualItemModel {
    ListItem {
        height: accountSettingsLayout.implicitHeight
        ListItemLayout {
            id: accountSettingsLayout
            title.text: qsTr("Manage accounts")

            Icon {
                height: units.gu(3)
                width: height
                name: "account"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
        onClicked: ViewActions.pushStage("qrc:/qml/views/stages/SettingsStage.qml", {})
    }
    ListItem {
        height: displaySettingsLayout.implicitHeight
        ListItemLayout {
            id: displaySettingsLayout
            title.text: qsTr("Display settings")

            Icon {
                height: units.gu(3)
                width: height
                name: "video-display-symbolic"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
        onClicked: {
            if (dekko.viewState.isSmallFF) {
                ViewActions.pushToStageArea(ViewKeys.messageListStack, "qrc:/qml/views/settings/DisplaySettingsPage.qml", {})
            } else {
                PopupUtils.open("qrc:/qml/views/settings/DisplaySettingsPopup.qml")
            }
        }
    }
    ListItem {
        height: privacySettingsLayout.implicitHeight
        ListItemLayout {
            id: privacySettingsLayout
            title.text: qsTr("Privacy settings")

            Icon {
                height: units.gu(3)
                width: height
                name: "private-browsing"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
        onClicked: {
            if (dekko.viewState.isSmallFF) {
                ViewActions.pushToStageArea(ViewKeys.messageListStack, "qrc:/qml/views/settings/PrivacySettingsPage.qml", {})
            } else {
                PopupUtils.open("qrc:/qml/views/settings/PrivacySettingsPopup.qml")
            }
        }
    }
    ListItem {
        height: offlineSettingsLayout.implicitHeight
        ListItemLayout {
            id: offlineSettingsLayout
            title.text: qsTr("Offline settings")

            Icon {
                height: units.gu(3)
                width: height
                name: "network-wifi-symbolic"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
    }
}


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
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Settings 1.0
import "../components"

DekkoPage {
    pageHeader.title: qsTr("Account Settings")
    pageHeader.backAction: Action {
        iconName: "back"
        onTriggered: {
            SettingsActions.saveCurrentGroup()
            if (SettingsStore.settingsChanged) {
                SettingsActions.saveSelectedAccount()
                ViewActions.reloadAccountBasedModels()
                SettingsStore.settingsChanged = false
            }
            ViewActions.popStageArea(ViewKeys.settingsStack2)
            ViewActions.popStageArea(ViewKeys.settingsStack1)
        }
    }

    pageHeader.primaryAction: Action {
        iconName: "delete"
        onTriggered: {
            AccountActions.deleteAccount(SettingsStore.selectedAccountId, true)
            ViewActions.popStageArea(ViewKeys.settingsStack2)
            ViewActions.popStageArea(ViewKeys.settingsStack1)
        }

    }

    PageFlickable {
        margins: 0
        spacing: 0
        ListItem {
            height: l1.height + divider.height
            ListItemLayout {
                id: l1
                title.text: qsTr("Details")
                ProgressionSlot{}
            }
            onClicked: SettingsActions.openSettingsGroup("qrc:/qml/views/settings/DetailsGroup.qml")
        }
        ListItem {
            height: l2.height + divider.height
            ListItemLayout {
                id: l2
                title.text: qsTr("Incoming Server")
                ProgressionSlot{}
            }
            onClicked: SettingsActions.openSettingsGroup("qrc:/qml/views/settings/IncomingServerGroup.qml")
        }
        ListItem {
            height: l3.height + divider.height
            ListItemLayout {
                id: l3
                title.text: qsTr("Outgoing Server")
                ProgressionSlot{}
            }
            onClicked: SettingsActions.openSettingsGroup("qrc:/qml/views/settings/OutgoingServerGroup.qml")
        }
        ListItem {
            height: l4.height + divider.height
            ListItemLayout {
                id: l4
                title.text: qsTr("Copies and Folders")
                ProgressionSlot{}
            }
            onClicked: SettingsActions.openSettingsGroup("qrc:/qml/views/settings/CopyFoldersGroup.qml")
        }
    }
}

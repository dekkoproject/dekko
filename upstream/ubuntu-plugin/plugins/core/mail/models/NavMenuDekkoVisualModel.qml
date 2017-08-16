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

VisualItemModel {
    ListItem {
        height: versionLayout.implicitHeight
        ListItemLayout {
            id: versionLayout
            title.text: qsTr("Version")

            Icon {
                height: units.gu(3)
                width: height
                name: "info"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
        onClicked: {
            PopupActions.queueDialog(PopupKeys.popupMainWindow, Qt.resolvedUrl("../dialogs/VersionDialog.qml"), {})
        }
    }
    ListItem {
        height: licenseLayout.implicitHeight
        ListItemLayout {
            id: licenseLayout
            title.text: qsTr("Licenses")

            Icon {
                height: units.gu(3)
                width: height
                name: "text-x-generic-symbolic"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
        onClicked: {
            ViewActions.pushToStageArea(ViewKeys.messageListStack, Qt.resolvedUrl("../LicensesPage.qml"), {})
        }
    }
    ListItem {
        height: contributorLayout.implicitHeight
        ListItemLayout {
            id: contributorLayout
            title.text: qsTr("Contributors")

            Icon {
                height: units.gu(3)
                width: height
                name: "contact"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
        onClicked: {
            ViewActions.pushToStageArea(ViewKeys.messageListStack, Qt.resolvedUrl("../ContributorsPage.qml"), {})
        }
    }
}


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
import Dekko.Mail.Accounts 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Views 1.0
import Dekko.Mail.Stores.Settings 1.0
import Dekko.Ubuntu.Components 1.0

DekkoPage {

    pageHeader.backAction: settingsStage.viewState.isSmallFF ? bk : null
    Action {
        id: bk
        iconName: "back"
        onTriggered: {
            SettingsActions.saveCurrentGroup()
            ViewActions.popStageArea(ViewKeys.settingsStack1)
        }
    }

    property Account account: SettingsStore.selectedAccount

    Connections {
        target: settingsStage.viewState
        onStateChanged: {
            if ((!settingsStage.viewState.isSmallFF) && (stageArea.stageID === ViewKeys.settingsStack1)) {
                SettingsActions.switchSettingsGroupLocation(ViewKeys.settingsStack1)
            } else if ((settingsStage.viewState.isSmallFF) && (stageArea.stageID === ViewKeys.settingsStack2)) {
                SettingsActions.switchSettingsGroupLocation(ViewKeys.settingsStack2)
            }
        }
    }
}

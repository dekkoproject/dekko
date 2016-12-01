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
import QtQml.StateMachine 1.0 as DSM
import QuickFlux 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Accounts 1.0

DSM.State {
    id: syncState

    property alias nextTargetState: next.targetState
    property alias quitTargetState: q.targetState

    onEntered: {
        Log.logStatus("SyncState::onEntered", "Loading sync overlay");
        if (stack.currentItem.objectName !== "addAnotherUI") {
            Log.logStatus("addAnotherUI::onEntered", "Pushing UserInputUI");
            WizardActions.wizardNavigateTo(Qt.resolvedUrl("../components/AddAnotherUI.qml"), {})
        }
    }

    onExited: {
    }

    DSM.SignalTransition {
        id: next
        signal: AccountSetup.goNext
    }

    DSM.SignalTransition {
        id: q
        signal: AccountSetup.quit
    }
}

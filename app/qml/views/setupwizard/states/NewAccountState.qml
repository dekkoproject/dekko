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
import Dekko.Mail 1.0
import "../../../actions/logging"
import "../../../actions/wizard"
import "../../../stores/accounts"

DSM.State {
    id: newAccountState

    property alias backTargetState: backTransition.targetState
    property alias nextTargetState: nextTransition.targetState

    onEntered: {
        Log.logStatus("NewAccountState::onEntered", "State entered")
        if (d.shouldPush) {
            Log.logInfo("NewAccountState::onEntered", "Pushing NewAccountsUI")
            WizardActions.wizardNavigateTo("qrc:/qml/views/setupwizard/components/NewAccountsUI.qml", {})
        } else {
            // we've come back to this state so reset the NewAccount in the store
            Log.logInfo("NewAccountState::onEntered", "Resetting account as user navigated back")
            WizardActions.removeNewAccount()
            WizardActions.wizardResetAccount()
        }
    }

    onExited: {
        Log.logInfo("NewAccountState::onExited", "Exited state")
    }

    DSM.SignalTransition {
        id: nextTransition
        signal: AccountSetup.goNext
    }

    DSM.SignalTransition {
        id: backTransition
        signal: AccountSetup.goBack
    }

    QtObject {
        id: d
        readonly property bool shouldPush: stack.currentItem === null || stack.currentItem.objectName !== "newAccountUI"
    }
}


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
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Accounts 1.0

DSM.State {
    id: noAccountsState


    property alias backTargetState: finishedTransition.targetState
    property alias createTargetState: createTransition.targetState

    onEntered: {
        Log.logStatus("NoAccountState::onEntered", "No accounts state entered");
        if (stack.currentItem === null) {
            Log.logStatus("NoAccountState::onEntered", "Pushing NoAccountsUI");
            WizardActions.wizardNavigateTo(Qt.resolvedUrl("../components/NoAccountsUI.qml"), {})
        }
    }

    onExited: {
        console.log("Exited no accounts state")
    }

    DSM.SignalTransition {
        id: createTransition
        signal: AccountSetup.createAccount
    }
    DSM.SignalTransition {
        id: finishedTransition
        signal: AccountSetup.goBack
    }
}

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
import Dekko.Controls 1.0
import Dekko.Mail 1.0
import QuickFlux 1.0
import "./states"
import "../../stores/accounts"
import "../../actions/logging"
import "../../actions/wizard"

Item {
    id: wizard

    anchors.fill: parent

    StackView {
        id: stack
        anchors.fill: parent
    }

    AppListener {
        filter: WizardKeys.wizardNavigateTo
        onDispatched: {
            stack.push({item: message.view, properties: message.properties});
        }
    }

    AppListener {
        filter: WizardKeys.wizardStepBack
        onDispatched: {
            // The navigate back action
            // will trigger the "quit" state when only 1 page left on the stack so no need
            // to pop anything here.
            if (stack.depth > 1) {
                Log.logInfo("SetupWizard::wizardStepBack", "Going back to previous step")
                stack.pop()
            }
            AccountSetup.goBack()
        }
    }

    DSM.StateMachine {
        initialState: Client.hasConfiguredAccounts ? newAccountState : noAccountsState
        running: true

        NoAccountState {
            id: noAccountsState
            backTargetState: quit
            createTargetState: newAccountState
        }

        NewAccountState {
            id: newAccountState
            // If we already have accounts configured then the user accessed this
            // from the "Manage accounts" UI so move to the quit state
            backTargetState: Client.hasConfiguredAccounts ? quit : noAccountsState
            // All account types have to go through the UserInputUI/State
            nextTargetState: userInputState
        }

        UserInputState {
            id: userInputState
            backTargetState: newAccountState
            // TODO: Move to SenderIdentity state if this is a preset account
            nextTargetState: AccountSetup.isPreset ? autoConfig : autoConfig
        }

        AutoConfigState {
            id: autoConfig
        }

        DSM.State {
            id: quit
            // FIXME: Use dispatcher API!!!
            onEntered: rootPageStack.pop()
        }

        DSM.FinalState {
            id: finished
        }
    }
}


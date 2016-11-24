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

/*For the setup wizard we use the state machine framework. http://doc.qt.io/qt-5/qmlstatemachine.html

  Each step of the setup wizard should be it's own state in the FSM and each state should be responsible
  for loading/activating the UI for that state.

  All states go into the setupwizard/states/ directory and UI views or components go into the setupwizard/components directory

  See http://code.dekkoproject.org/dekko-dev/dekko/uploads/626fa76f365226af0b3fa84d39c7e589/SetupWizardStateMachine.pdf
  for a diagram of the intended state transitions.
*/
Item {
    id: wizard

    anchors.fill: parent
    // Use stack view so we get the nice page transitions
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

    AppListener {
        filter: WizardKeys.addAnotherAccount
        onDispatched: {
            WizardActions.wizardResetAccount()
            stack.clear()
            WizardActions.wizardStepForward()
        }
    }

    // Here we define our statemachine and there next/previous states
    DSM.StateMachine {
        // TODO: Use the AccountsStore to query configured accounts. views/ shouldn't talk to Client directly
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
            // TODO: same as initialState todo
            backTargetState: Client.hasConfiguredAccounts ? quit : noAccountsState
            // All account types have to go through the UserInputUI/State
            nextTargetState: userInputState
        }

        UserInputState {
            id: userInputState
            backTargetState: newAccountState
            // TODO: Move to validation state if this is a preset account
            nextTargetState: AccountSetup.isPreset ? validate : autoConfig
        }

        OhNoTheyWantAGoogleAccountState {
            id: yukItsGoogle
        }

        AutoConfigState {
            id: autoConfig
            nextTargetState: AccountSetup.needsValidation ? validate : manualInput
        }

        ManualInputState {
            id: manualInput
            nextTargetState: validate
            backTartgetState: userInputState
        }

        ValidationState {
            id: validate
            backTartgetState: manualInput
            nextTargetState: sync
        }

        SyncState {
            id: sync
            nextTargetState: addAnother
        }

        AddAnotherState {
            id: addAnother
            nextTargetState: newAccountState
            quitTargetState: finished
        }

        // This state deletes the NewAccount object
        // and removes it form the accounts store if it exists
        // i.e we tried validating the account which requires
        // we save it first so it has a valid entry in the db
        // that needs clenaing up
        DSM.State {
            id: quit
            onEntered: {
                WizardActions.removeNewAccount()
                WizardActions.wizardResetAccount()
                rootPageStack.pop()
            }
        }

        DSM.FinalState {
            id: finished
        }
        onFinished: {
            WizardActions.wizardResetAccount()
            // FIXME: Use dispatcher API!!!
            rootPageStack.pop()
        }
    }
}


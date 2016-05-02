import QtQuick 2.4
import QtQml.StateMachine 1.0 as DSM
import Dekko.Controls 1.0
import Dekko.Mail 1.0
import QuickFlux 1.0
import "./states"
import "../../stores"
import "../../actions"

Item {
    id: wizard

    anchors.fill: parent

    StackView {
        id: stack
        anchors.fill: parent
    }

    AppListener {
        filter: ActionTypes.wizardNavigateTo
        onDispatched: {
            stack.push({item: message.view, properties: message.properties});
        }
    }

    AppListener {
        filter: ActionTypes.wizardStepBack
        onDispatched: {
            // The navigate back action
            // will trigger the "quit" state when only 1 page left on the stack so no need
            // to pop anything here.
            if (stack.depth > 1) {
                DekkoActions.logInfo("SetupWizard::wizardStepBack", "Going back to previous step")
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
            backTargetState: Client.hasConfiguredAccounts ? quit : noAccountsState
            nextTargetState: userInputState
        }

        UserInputState {
            id: userInputState
            backTargetState: newAccountState
        }

        DSM.State {
            id: quit
            onEntered: rootPageStack.pop()
        }

        DSM.FinalState {
            id: finished
        }
    }
}


import QtQuick 2.4
import QtQml.StateMachine 1.0 as DSM
import "../../../actions/logging"
import "../../../actions/wizard"
import "../../../stores"

DSM.State {
    id: noAccountsState


    property alias backTargetState: finishedTransition.targetState
    property alias createTargetState: createTransition.targetState

    onEntered: {
        Log.logStatus("NoAccountState::onEntered", "No accounts state entered");
        if (stack.currentItem === null) {
            Log.logStatus("NoAccountState::onEntered", "Pushing NoAccountsUI");
            WizardActions.wizardNavigateTo("qrc:/qml/views/setupwizard/components/NoAccountsUI.qml", {})
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

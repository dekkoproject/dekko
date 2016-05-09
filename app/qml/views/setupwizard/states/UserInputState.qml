import QtQuick 2.4
import QtQml.StateMachine 1.0 as DSM
import "../../../actions/logging"
import "../../../actions/wizard"
import "../../../stores/accounts"

DSM.State {
    id: userInputState

    property alias backTargetState: finishedTransition.targetState
    property alias nextTargetState: nextTargetState.targetState

    onEntered: {
        Log.logStatus("UserInputState::onEntered", "User details input state entered");
        if (stack.currentItem.objectName !== "userInputUI") {
            Log.logStatus("NoAccountState::onEntered", "Pushing UserInputUI");
            WizardActions.wizardNavigateTo("qrc:/qml/views/setupwizard/components/UserInputUI.qml", {})
        }
    }

    onExited: {
        console.log("Exited no accounts state")
    }

    DSM.SignalTransition {
        id: nextTargetState
        signal: AccountSetup.goNext
    }

    DSM.SignalTransition {
        id: finishedTransition
        signal: AccountSetup.goBack
    }
}

import QtQuick 2.4
import QtQml.StateMachine 1.0 as DSM
import "../../actions"
import "../../stores"

DSM.State {
    id: userInputState

    property alias backTargetState: finishedTransition.targetState
    property alias nextTargetState: nextTargetState.targetState

    onEntered: {
        DekkoActions.logStatus("UserInputState::onEntered", "User details input state entered");
        if (stack.currentItem.objectName !== "userInputUI") {
            DekkoActions.logStatus("NoAccountState::onEntered", "Pushing UserInputUI");
            DekkoActions.wizardNavigateTo("qrc:/qml/setupwizard/components/UserInputUI.qml", {})
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

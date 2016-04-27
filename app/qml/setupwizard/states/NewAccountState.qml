import QtQuick 2.4
import QtQml.StateMachine 1.0 as DSM
import Dekko.Mail 1.0
import "../../actions"
import "../../stores"

DSM.State {
    id: newAccountState

    property alias backTargetState: backTransition.targetState
    property alias nextTargetState: nextTransition.targetState

    onEntered: {
        DekkoActions.logStatus("NewAccountState::onEntered", "State entered")
        if (d.shouldPush) {
            DekkoActions.logInfo("NewAccountState::onEntered", "Pushing NewAccountsUI")
            DekkoActions.wizardNavigateTo("qrc:/qml/setupwizard/components/NewAccountsUI.qml", {})
        } else {
            // we've come back to this state so reset the NewAccount in the store
            DekkoActions.logInfo("NewAccountState::onEntered", "Resetting account as user navigated back")
            DekkoActions.wizardResetAccount()
        }
    }

    onExited: {
        DekkoActions.logInfo("NewAccountState::onExited", "Exited state")
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


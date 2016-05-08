pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Mail 1.0
import "../actions/views"
import "../actions/logging"
import "./"

AppListener {
    id: viewStore

    property string formFactor

    Filter {
        type: ViewKeys.stageStackReady
        onDispatched: {
            Log.logStatus("ViewStore::stageStackReady", "Stack is ready")
            ViewActions.pushStage("qrc:/qml/views/stages/MailStage.qml", {})
            if (!Client.hasConfiguredAccounts) {
                ViewActions.runSetupWizard()
            }
        }
    }

    Filter {
        type: ViewKeys.runSetupWizard
        onDispatched: {
            ViewActions.pushStage("qrc:/qml/views/setupwizard/SetupWizard.qml", {})
        }
    }
}


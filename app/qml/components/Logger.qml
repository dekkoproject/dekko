import QtQuick 2.4
import QuickFlux 1.0
import "../actions"

AppListener {
    id: logListener

    Filter {
        type: ActionTypes.logInfo
        onDispatched: {
            logMessage(message.location, "INFO", message.message)
        }
    }
    Filter {
        type: ActionTypes.logError
        onDispatched: {
            logMessage(message.location, "ERROR", message.message)
        }
    }
    Filter {
        type: ActionTypes.logStatus
        onDispatched: {
            logMessage(message.location, "STATUS", message.message)
        }
    }
    Filter {
        type: ActionTypes.logWarning
        onDispatched: {
            logMessage(message.location, "WARNING", message.message)
        }
    }
    function logMessage(location, type, message) {
        console.log("[%1] [%2] %3".arg(location).arg(type).arg(message))
    }
}


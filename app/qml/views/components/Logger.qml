import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Components 1.0
import "../../actions/logging"

AppListener {
    id: logListener
    property bool devLoggingEnabled: true

    Filter {
        type: LogKeys.logInfo
        onDispatched: {
            if (verboseLogging) {
                logMessage(message.location, "INFO", message.message)
            }
        }
    }
    Filter {
        type: LogKeys.logStatus
        onDispatched: {
            if (verboseLogging) {
                logMessage(message.location, "STATUS", message.message)
            }
        }
    }
    Filter {
        type: LogKeys.logWarning
        onDispatched: {
            logMessage(message.location, "WARNING", message.message)
        }
    }
    Filter {
        type: LogKeys.logError
        onDispatched: {
            logMessage(message.location, "ERROR", message.message)
        }
    }

    function logMessage(location, type, message) {
        if (devLoggingEnabled) {
            switch (type) {
            case "INFO":
                LogRecorder.logMessage(location, LogRecorder.INFO, message)
                break
            case "ERROR":
                LogRecorder.logMessage(location, LogRecorder.ERROR, message)
                break
            case "WARNING":
                LogRecorder.logMessage(location, LogRecorder.WARNING, message)
                break
            case "STATUS":
                LogRecorder.logMessage(location, LogRecorder.STATUS, message)
                break
            }
        }
        console.log("[%1] [%2] %3".arg(location).arg(type).arg(message))
    }
}


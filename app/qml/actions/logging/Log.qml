pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0

ActionCreator {
    //=========================================================================\\
    // Logging helpers                                                         \\
    // These are handled in components/Logger.qml                              \\
    //=========================================================================\\
    signal logInfo(string location, string message)
    signal logStatus(string location, string message)
    signal logWarning(string location, string message)
    signal logError(string location, string message)

}

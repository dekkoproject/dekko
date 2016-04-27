pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0
import "../actions"

AppListener {

    property bool showCompletedTasks: false

    Filter {
        // Views do not write to showCompeletedTasks directly.
        // It asks AppActions.setShowCompletedTasks() to do so.
        type: ActionTypes.setShowCompleted
        onDispatched: {
            console.log("Show completed tasks dispatched")
            showCompletedTasks = message.value;
        }
    }
}

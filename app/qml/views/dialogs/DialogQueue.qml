import QtQuick 2.4
import QuickFlux 1.0
import "../../actions"
import "../utils"

AppListener {
    id: logListener

    property PopupQueue popupQueue: PopupQueue {}

    Filter {
        type: ActionTypes.showError
        onDispatched: {
            popupQueue.queue("qrc:/qml/views/popovers/NoticePopup.qml", dekko, {title: qsTr("Error"), text: message.error})
        }
    }

    Filter {
        type: ActionTypes.queueDialog
        onDispatched: {
            popupQueue.queue(message.dlg, dekko, message.properties)
        }
    }

    Filter {
        type: ActionTypes.showConfirmationDialog
        onDispatched: {
            popupQueue.queue("qrc:/qml/views/dialogs/ConfirmationDialog.qml", dekko, {id: message.id, title: message.title, text: message.text})
        }
    }
}


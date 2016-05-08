import QtQuick 2.4
import QuickFlux 1.0
import "../../actions"
import "../../actions/popups"
import "../utils"

AppListener {
    id: logListener

    property PopupQueue popupQueue: PopupQueue {}

    Filter {
        type: PopupKeys.showError
        onDispatched: {
            popupQueue.queue("qrc:/qml/views/popovers/NoticePopup.qml", dekko, {title: qsTr("Error"), text: message.error})
        }
    }

    Filter {
        type: PopupKeys.showNotice
        onDispatched: {
            popupQueue.queue("qrc:/qml/views/popovers/NoticePopup.qml", dekko, {title: qsTr("Notice"), text: message.notice})
        }
    }

    Filter {
        type: PopupKeys.queueDialog
        onDispatched: {
            popupQueue.queue(message.dlg, dekko, message.properties)
        }
    }

    Filter {
        type: PopupKeys.showConfirmationDialog
        onDispatched: {
            popupQueue.queue("qrc:/qml/views/dialogs/ConfirmationDialog.qml", dekko, {id: message.id, title: message.title, text: message.text})
        }
    }
}


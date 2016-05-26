import QtQuick 2.4
import QuickFlux 1.0
import "../../actions/views"

AppListener {

    property ToastQueue toaster: ToastQueue{}

    Filter {
        type: ViewKeys.orderSimpleToast
        onDispatched: {
            toaster.orderToast(dekko, simple, { text: message.message })
        }
    }

    Component {
        id: simple
        SimpleToast {}
    }
}


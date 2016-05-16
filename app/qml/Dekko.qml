import QtQuick 2.4
import Ubuntu.Components 1.3
import "./actions/views"
import "./actions/logging"
import "./views/components"
import "./views/utils"
import "./views/stages"
import "./stores"

Item {
    id: dekko
    property alias viewState: view
    anchors.fill: parent
    ViewState {
        id: view
        anchors.fill: parent
        onStateChanged: {
            ViewStore.formFactor = state
            Log.logStatus("ViewState::stateChanged", state)
        }
    }

    Item {
        id: dekkoContainer
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
            // anchor to the top of KeyboardRectangle
            // this ensures pages are always above the OSK
            // Basically the same as anchorToKeyboard
            bottom: kbdRect.top
        }
        StageStack {
            id: rootPageStack
            Component.onCompleted: {
                ViewActions.stageStackReady()
            }
        }
    }

    KeyboardRectangle {
        id: kbdRect
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}


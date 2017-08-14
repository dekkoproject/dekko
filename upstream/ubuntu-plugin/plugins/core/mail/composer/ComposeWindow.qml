import QtQuick 2.4
import QtQuick.Window 2.1
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Composer 1.0
import MazDB 1.0
import PlugMan 1.0
import QuickFlux 1.0
import Dekko.Ubuntu.Components 1.0

Window {
    id: window
    visible: true
    minimumWidth: units.gu(30)
    minimumHeight: units.gu(30)
    width: units.gu(60)
    height: units.gu(60)
    flags: Qt.Window
    modality: Qt.NonModal

    title: qsTr("Dekko Composer")

    onClosing: {
        if (ComposerStore.builder.hasDraft) {
            close.accepted = false
            ComposerActions.discardMessage()
        } else {
            ComposerActions.resetComposer()
            ViewActions.closeComposer()
        }
    }

    MazDBSettings {
        category: "composer-property-cache"
        property alias windowWidth: window.width
        property alias windowHeight: window.height
        property alias windowX: window.x
        property alias windowY: window.y
    }

    Item {
        anchors.fill: parent

        Item {
            id: container

            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
                bottom: kbdRect.top
            }

            ItemRegistry {
                target: container
                location: "Dekko::Mail::Composer::UI"
                loadMode: ItemRegistry.LoadFirstEnabled
            }

            AppListener {
                filter: PopupKeys.showDiscardDialog
                onDispatched: {
                    PopupUtils.open(Qt.resolvedUrl("../dialogs/ConfirmationDialog.qml"), container, {id: message.id, title: message.title, text: message.text})
                }
            }
        }
        KeyboardRectangle {
            id: kbdRect
            anchors {
                left: parent.left
                bottom: parent.bottom
                right: parent.right
            }
        }
    }
}

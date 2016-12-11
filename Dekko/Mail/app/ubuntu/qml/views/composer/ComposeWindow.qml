import QtQuick 2.4
import QtQuick.Window 2.1
import Ubuntu.Components 1.3
import Dekko.Mail.API 1.0
import MazDB 1.0
import PlugMan 1.0

Window {
    id: window
    visible: true
    minimumWidth: units.gu(30)
    minimumHeight: units.gu(30)
    flags: Qt.Window
    modality: Qt.NonModal

    title: qsTr("Dekko Composer")

    onClosing: { ViewActions.closeComposer() }

    MazDBSettings {
        category: "composer-property-cache"
        property alias windowWidth: window.width
        property alias windowHeight: window.height
        property alias windowX: window.x
        property alias windowY: window.y
    }

    Item {
        id: container
        anchors.fill: parent
        ItemRegistry {
            target: container
            location: "Dekko::Mail::Composer::UI"
            loadMode: ItemRegistry.LoadFirstEnabled
        }
    }
}

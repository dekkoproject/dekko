import QtQuick 2.4
import Ubuntu.Components 1.3
Rectangle {
    width: units.gu(40)
    height: width
    color: UbuntuColors.red
    Timer {
        interval: 500
        triggeredOnStart: true
        repeat: true
        onTriggered: console.log("Plugin timer triggered")
        Component.onCompleted: start()
    }
}

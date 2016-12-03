import QtQuick 2.4
import Ubuntu.Components 1.3
import "../src"


MainView {
    id: mainView
    width: units.gu(100)
    height: units.gu(70)

    Page {
        title: "Notify example"

        Column {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: units.gu(10)
            }
            spacing: units.gu(1.5)
            Button {
                width: parent.width
                text: "Notify"
                onClicked: notify.send("Hello notification")
            }
            Button {
                width: parent.width
                text: "Notify with body"
                onClicked: notify.send("Hello notification", "With notification body, you can add a bit more context here.")
            }
            Button {
                width: parent.width
                text: "Notify on left (Long display time. 6 secs)"
                onClicked: notify.send("Hello notification", "From the left", leftConfig)

                NotifyConfiguration {
                    id: leftConfig
                    position: Item.Left
                    displayTime: 6000
                }
            }
            Button {
                width: parent.width
                text: "Notify in center (short display time. 1.5 sec)"
                onClicked: notify.send("Hello notification", "I'm in the center!", centerConfig)

                NotifyConfiguration {
                    id: centerConfig
                    position: Item.Center
                    displayTime: 1500
                }
            }
            Button {
                width: parent.width
                text: "Notify with action"
                onClicked: notify.send("Hello notification", "Click me to trigger action", actionConfig)
            }
            Button {
                width: parent.width
                text: "Notify no body with action"
                onClicked: notify.send("Hello notification", actionConfig)
            }
            Button {
                width: parent.width
                text: "Notify with Icon"
                onClicked: notify.send("Hello notification", iconConfig)

                NotifyConfiguration {
                    id: iconConfig
                    iconName: "edit-clear"
                    iconColor: UbuntuColors.red
                }
            }
            Button {
                width: parent.width
                text: "Custom notification"
                onClicked: notify.send("Custom notify", customConfig)

                NotifyConfiguration {
                    id: customConfig
                    source: Qt.resolvedUrl("./CustomNotification.qml")
                }
            }
        }

        NotifyConfiguration {
            id: actionConfig
            action: Action {
                onTriggered: {
                    notify.send("Action triggered!")
                }
            }
        }
    }

    Notify {
        id: notify
        anchors.fill: parent
    }
}

import QtQuick 2.4
import Ubuntu.Components 1.3
import Plugins 0.1

MainView {
    height: units.gu(70)
    width: height

    ActionRegistry {
        id: actionRegistry
        location: "Composer::ToolbarAction"
        defaultActions: [
            Action {
                iconName: "ok"
                onTriggered: console.log("Default action triggered")
            },
            Action {
                iconName: "back"
                onTriggered: console.log("Not going back now!")
            }
        ]
    }

    ItemRegistry {
        target: col
        asynchronous: true
        location: "View::Item"
        defaultItems: [
            Label {
                text: "Default label"
                width: parent.width
            },
            Label {
                text: "Default label 2"
                width: parent.width
            },
            Label {
                text: "Default label 3"
                width: parent.width
            }
        ]
    }

    ListenerRegistry {
        defaultListeners: [
            Rectangle {
                width: units.gu(40)
                height: width
                color: UbuntuColors.red
                Timer {
                    interval: 1000
                    triggeredOnStart: true
                    repeat: true
                    onTriggered: console.log("Timer triggered")
                    Component.onCompleted: {
                        start()
                    }
                }
            }
        ]
    }

    Page {
        header: PageHeader {
            title: "Page Header"
            trailingActionBar.actions: actionRegistry.actions
        }

        Rectangle {
            color: UbuntuColors.porcelain
            anchors.centerIn: parent
            height: units.gu(50)
            width: height
            Column {
                id: col
                anchors {
                    left: parent.left
                    top: parent.top
                    right: parent.right
                    margins: units.gu(2)
                }
                spacing: units.gu(1)
            }
        }
    }
}


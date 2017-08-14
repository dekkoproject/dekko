import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail.API 1.0
import Dekko.Components 1.0
import PlugMan 1.0
import MazDB 1.0
import Dekko.Ubuntu.Components 1.0

Rectangle {
    id: bar
    color: UbuntuColors.inkstone

    property int currentIndex: 0

    // Mail stage is special here as it's the only one kept on the stack
    // all other stage plugins are destroyed on stage push's
    // If you want your stage to maintain state then you should make
    // use of the store/worker pattern and use MazDB or MazDBSettings for data persistence
    ActionRegistry {
        id: navRegistry
        location: "Dekko::Stage::Action"
        defaultActions: [
            Action {
                iconName: "mail-unread"
                onTriggered: ViewActions.rewindStageStack()
            },
            Action {
                iconName: "address-book-app-symbolic"
                onTriggered: ViewActions.pushStage("./AddressBookStage.qml", {})
            },
            Action {
                iconName: "calendar"
                onTriggered: ViewActions.pushStage("./CalendarStage.qml", {})
            },
            Action {
                iconName: "stock_note"
                onTriggered: ViewActions.pushStage("./NotesStage.qml", {})
            },
            Action {
                iconName: "attachment"
                onTriggered: ViewActions.pushStage("./AttachmentsStage.qml", {})
            }
        ]
    }

    Line {
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        color: UbuntuColors.slate
    }

    StretchColumn {
        anchors.fill: parent

        AbstractButton {
            id: drawerBtn
            height: units.gu(6)
            width: bar.width
            implicitHeight: height
            visible: !dekko.isLargeFF

            onClicked: ViewActions.toggleNavDrawer()

            Rectangle {
                anchors.fill: parent
                visible: drawerBtn.pressed
                color: UbuntuColors.slate
            }

            Icon {
                name: "navigation-menu"
                color: UbuntuColors.silk
                height: units.gu(3)
                width: height
                anchors.centerIn: parent
            }

        }

        Line {
            color: UbuntuColors.slate
            visible: !dekko.isLargeFF
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: units.gu(1)
                rightMargin: units.gu(1)
            }
        }

        Stretcher {
            clip: true
            Flickable {
                anchors.fill: parent
                contentHeight: col.height + units.gu(5)
                Column {
                    id: col
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                    }

                    Repeater {
                        model: navRegistry.actions

                        delegate: AbstractButton {
                            id: btn

                            readonly property bool isSelected: bar.currentIndex === index

                            action: modelData
                            height: units.gu(7)
                            width: bar.width
                            implicitHeight: height
                            visible: action.visible

                            onClicked: bar.currentIndex = index

                            Rectangle {
                                anchors.fill: parent
                                visible: btn.pressed
                                color: UbuntuColors.slate
                            }

                            Rectangle {
                                color: UbuntuColors.blue
                                width: units.dp(2)
                                anchors {
                                    left: parent.left
                                    top: parent.top
                                    bottom: parent.bottom
                                }
                                visible: btn.isSelected
                            }

                            Icon {
                                name: action.iconName
                                color: btn.isSelected ? UbuntuColors.blue : UbuntuColors.silk
                                height: units.gu(3.4)
                                width: height
                                anchors.centerIn: parent
                            }
                        }
                    }
                }
            }
        }

        Line {
            color: UbuntuColors.slate
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: units.gu(1)
                rightMargin: units.gu(1)
            }
        }

        AbstractButton {
            height: units.gu(7)
            width: bar.width
            implicitHeight: height

            Icon {
                name: "like"
                color: UbuntuColors.green
                height: units.gu(3.4)
                width: height
                anchors.centerIn: parent
            }
        }

        AbstractButton {
            id: settingsBtn
            height: units.gu(7)
            width: bar.width
            implicitHeight: height

            onClicked: ViewActions.openSettings()

            Rectangle {
                anchors.fill: parent
                visible: settingsBtn.pressed
                color: UbuntuColors.slate
            }

            Icon {
                name: "settings"
                color: UbuntuColors.silk
                height: units.gu(3.4)
                width: height
                anchors.centerIn: parent
            }
        }
    }
}

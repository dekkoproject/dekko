import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail 1.0
import Dekko.Components 1.0
import "../components"
import "../delegates"

DekkoPage {
    id: contactFilterView

    property string title: ""
    property alias filterKey: msgList.messageKey
    property string name: title
    property string email: ""
    property string initials: ""

    pageHeader.title: title
    pageHeader.backAction: Action {
        iconName: "back"
        onTriggered: rootPageStack.pop()
    }

    MessageList {
        id: msgList
        sortOrder: Qt.DescendingOrder
        filter: MessageList.All
    }

    ListItem {
        id: contactHeader
        anchors {
            left: parent.left
            top: pageHeader.bottom
            right: parent.right
        }

        height: c.height + units.gu(2)

        Column {
            id: c
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            spacing: units.gu(1)

            Item {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: avatar.height + units.gu(3)
                Avatar {
                    id: avatar
                    anchors {
                        centerIn: parent
                    }
                    width: units.gu(10)
                    height: width
                    fontSize: "x-large"
                    name: contactFilterView.name
                    email: contactFilterView.email
                    initials: contactFilterView.initials
                }
            }
            Item {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: addressLabel.height
                Label {
                    id: addressLabel
                    anchors {
                        centerIn: parent
                    }
                    text: name ? email : ""
                }
            }

            Item {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: addEditButton.height

                Button {
                    id: addEditButton
                    text: qsTr("Add contact")
                    width: parent.width / 2 - units.gu(4)
                    anchors {
                        left: parent.left
                        leftMargin: units.gu(2)
                    }
                    color: UbuntuColors.green
                }
                Button {
                    id: sendButton
                    text: qsTr("Send message")
                    width: parent.width / 2 - units.gu(1)
                    anchors {
                        right: parent.right
                        rightMargin: units.gu(2)
                    }
                    color: UbuntuColors.green
                }
            }
        }
    }

    Animations {
        id: animations
    }

    ScrollView {
        anchors {
            left: parent.left
            top: contactHeader.bottom
            right: parent.right
            bottom: parent.bottom
        }

        ListView {
            anchors.fill: parent
            clip: true
            add: animations.listViewAddTransition
            addDisplaced: animations.listViewAddDisplacedTransition
            remove: animations.listViewRemoveTransition
            removeDisplaced: animations.listViewRemoveDisplacedTransition
            model: msgList.model
            delegate: MessageListDelegate {
                id: msgListDelegate
                anchors {
                    left: parent.left
                    right: parent.right
                }
                avatarVisible: false
                msg: model.qtObject
                leftSideAction: Action {
                    iconName: "delete"
                    onTriggered: {
                        Client.deleteMessage(msgListDelegate.msg.messageId)
                    }
                }
                rightSideActions: [flagAction, readAction, moveAction, contextAction]
            }

            Component.onCompleted: positionViewAtBeginning()
        }
    }
}


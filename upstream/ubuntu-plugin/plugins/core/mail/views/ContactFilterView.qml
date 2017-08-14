/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu devices

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail 1.0
import Dekko.Mail.API 1.0
import Dekko.Components 1.0
import Dekko.Ubuntu.Components 1.0
import "./delegates"
import Dekko.Ubuntu.Constants 1.0

DekkoPage {
    id: contactFilterView

    property var sender
    property string title: ""
    property alias filterKey: msgList.messageKey
    property string name: title
    property string email: ""
    property string initials: ""

    pageHeader.title: title
    pageHeader.backAction: Action {
        iconName: "back"
        onTriggered: ViewActions.popStageArea(ViewKeys.messageListStack)
    }
    extendHeader: !dekko.isSmallFF

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

        height: c.height + Style.defaultSpacing

        Column {
            id: c
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            spacing: Style.smallSpacing

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
                    width: parent.width / 2 - Style.largeSpacing
                    anchors {
                        left: parent.left
                        leftMargin: Style.defaultSpacing
                    }
                    color: UbuntuColors.green
                    onTriggered: ViewActions.openUri(Uris.newContactUri(sender.completeAddress))
                }
                Button {
                    id: sendButton
                    text: qsTr("Send message")
                    width: parent.width / 2 - Style.smallSpacing
                    anchors {
                        right: parent.right
                        rightMargin: Style.defaultSpacing
                    }
                    color: UbuntuColors.green
                    onTriggered: ViewActions.openUri(sender.toRfc6068Mailto())
                }
            }
        }
    }

    ScrollView {
        anchors {
            left: parent.left
            top: contactHeader.bottom
            right: parent.right
            bottom: parent.bottom
        }

        ListView {
            id: lv
            anchors.fill: parent
            clip: true
            add: DekkoAnimation.listViewAddTransition
            addDisplaced: DekkoAnimation.listViewAddDisplacedTransition
            remove: DekkoAnimation.listViewRemoveTransition
            removeDisplaced: DekkoAnimation.listViewRemoveDisplacedTransition
            highlight: !dekko.isSmallFF ? Style.highlightBar : null
            highlightFollowsCurrentItem: true
            highlightMoveDuration: 200
            model: msgList.model
            currentIndex: -1
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
                        MessageActions.deleteMessage(msgListDelegate.msg.messageId)
                    }
                }
                rightSideActions: [flagAction, readAction, contextAction]
                onItemClicked: {
                    if (mouse.button === Qt.RightButton) {
                        rightClickActions.trigger()
                        return;
                    }
                    lv.currentIndex = model.index
                    MessageActions.openMessage(msgListDelegate.msg.messageId)
                }
            }

            Component.onCompleted: positionViewAtBeginning()
        }
    }
}


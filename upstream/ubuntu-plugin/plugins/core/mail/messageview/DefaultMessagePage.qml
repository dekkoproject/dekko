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
import Ubuntu.Components.Popups 1.3
import com.canonical.Oxide 1.9 as Oxide
import Dekko.Mail 1.0
import Dekko.Components 1.0
import Dekko.Mail.Settings 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Views 1.0
import Dekko.Ubuntu.Components 1.0
import "../components"
import "../webview"
import "../composer"
import Dekko.Ubuntu.Constants 1.0

DekkoPage {

    // Hide the default header
    pageHeader.showDivider: false
    pageHeader.visible: false

    DefaultMessagePageHeader {
        id: msgPgHeader
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        msg: message
    }

    property alias msgId: message.messageId

    Connections {
        target: ViewStore
        onFormFactorChanged: {
            if ((ViewStore.formFactor !== "small") && (stageArea.stageID === ViewKeys.messageListStack)) {
                ViewActions.switchMessageViewLocation(ViewKeys.messageListStack, msgId)
            } else if ((ViewStore.formFactor === "small") && (stageArea.stageID === ViewKeys.messageViewStack)) {
                ViewActions.switchMessageViewLocation(ViewKeys.messageViewStack, msgId)
            }
        }
    }

    Message {
        id: message
        onBodyChanged: {
            webview.setCidQuery(message.messageId)
            webview.setBodyUrl(body)
            if (!message.isRead) {
                switch(mailPolicy.markRead) {
                case MailPolicy.Never:
                    Log.logInfo("DefaultMessagePage::onBodyChanged", "Not marking message as read");
                    break; // do nothing
                case MailPolicy.AfterInterval:
                    Log.logInfo("DefaultMessagePage::onBodyChanged", "Marking message read after interval");
                    markReadTimer.start()
                    break
                case MailPolicy.Immediately:
                    Log.logInfo("DefaultMessagePage::onBodyChanged", "Marking message read immediately");
                    MessageActions.markMessageRead(message.messageId, true)
                    break
                }
            }
        }
    }

    MailPolicy {
        id: mailPolicy
        accountId: message.parentAccountId
    }

    Timer {
        id: markReadTimer
        interval: mailPolicy.markAsReadInterval
        repeat: false
        onTriggered: MessageActions.markMessageRead(message.messageId, true)
    }

    ContentBlockedNotice {
        id: contentBlockedItem
        contentBlocked: webview.contentBlocked
        anchors {
            left: parent.left
            top: msgPgHeader.bottom
            right: parent.right
        }
        onAllowClicked: webview.allowBlockedContent()
    }

    Item {
        id: container
        clip: true
        anchors {
            left: parent.left
            right: parent.right
            top: contentBlockedItem.bottom
            bottom: hiddenMargin.top
        }

        DekkoWebView {
            id: webview
            anchors {
                fill: parent
                leftMargin: !dekko.isSmallFF ? Style.smallSpacing : 0
            }
            locationBarController.mode: Oxide.LocationBarController.ModeAuto
            locationBarController.height: h.height
        }

        MessageHeader {
            id: h
            msg: message
            detailsVisible: detailsPanel.showDetails
            width: parent.width
            y: webview ? webview.locationBarController.offset : 0
            onShowDetails: detailsPanel.showDetails = !detailsPanel.showDetails
        }

        ListItem {
            id: detailsPanel
            property bool showDetails: false
            property int detailsHeight: details.height > units.gu(30) ? units.gu(30) : details.height
            color: "#ffffff"
            anchors {
                left: parent.left
                right: parent.right
                top: h.bottom
            }
            height: 0
            expansion.height: detailsHeight
            expansion.expanded: showDetails
            divider.visible: showDetails
            Flickable {
                anchors.fill: parent
                contentHeight: details.height
                Column {
                    id: details
                    anchors {
                        left: parent.left
                        leftMargin: Style.smallSpacing
                        right: parent.right
                        rightMargin: Style.smallSpacing
                        top: parent.top
                    }
                    ListItem {
                        height: units.gu(5)
                        Item {
                            id: p
                            width: tl.width + Style.smallSpacing
                            anchors {
                                left: parent.left
                                top: parent.top
                                bottom: parent.bottom
                            }

                            Label {
                                id: tl
                                anchors {
                                    left: parent.left
                                    top: parent.top
                                    topMargin: Style.defaultSpacing
                                    leftMargin: Style.smallSpacing
                                }
                                text: qsTr("From:")
                            }
                        }
                        UbuntuShape {
                            id: delegate
                            aspect: UbuntuShape.Flat
                            color: Qt.rgba(0, 0, 0, 0.05)
                            radius: "small"
                            height: units.gu(3)
                            width: inner_avatar.width + label.width + units.gu(1.5)
                            anchors {
                                left: p.right
                                leftMargin: Style.smallSpacing
                                top: parent.top
                                topMargin: units.gu(1.5)
                            }

                            Avatar {
                                id: inner_avatar
                                width: height
                                anchors {
                                    left: parent.left
                                    top: parent.top
                                    bottom: parent.bottom
                                    margins: units.dp(1)
                                }
                                name: message.from.name
                                initials: message.from.initials
                                email: message.from.address
                                fontSize: "x-small"
                                validContact: true
                            }

                            Label {
                                id: label
                                anchors {
                                    left: inner_avatar.right
                                    leftMargin: units.gu(0.5)
                                    verticalCenter: parent.verticalCenter
                                }
                                text: message.from.name
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    PopupUtils.open(Qt.resolvedUrl("../popovers/RecipientPopover.qml"), delegate, {address: message.from})
                                }
                            }
                        }
                    }

                    RecipientFlow {
                        title: qsTr("To:")
                        model: message.to
                    }
                    RecipientFlow {
                        visible: message.cc.count
                        title: qsTr("Cc:")
                        model: message.cc
                    }
                }
            }
        }
    }
    Item {
        id: hiddenMargin
        anchors {
            left: parent.left
            bottom: parent.bottom
            right: parent.right
        }
        height: attachmentPanel.visible ? units.gu(6) : 0
    }

    AttachmentPanel {
        id: attachmentPanel
        attachments: message.attachments
        anchors {
            left: parent.left
            bottom: parent.bottom
            right: parent.right
        }
        maxHeight: container.height - Style.defaultSpacing
    }
}


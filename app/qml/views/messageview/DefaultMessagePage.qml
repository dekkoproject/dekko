import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import com.canonical.Oxide 1.9 as Oxide
import Dekko.Mail 1.0
import Dekko.Components 1.0
import "../../components"
import "../../webview"

DekkoPage {

    DefaultMessagePageHeader {
        id: msgPgHeader
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        msg: message
    }
    // Hide the default header
    pageHeader.showDivider: false

    property alias msgId: message.messageId

    Message {
        id: message
        onBodyChanged: {
            //            console.log("MainPartUrl: ", body)
            webview.setCidQuery(message.messageId)
            webview.setBodyUrl(body)
        }
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
                leftMargin: !dekko.viewState.isSmallFF ? units.gu(1) : 0
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
                        leftMargin: units.gu(1)
                        right: parent.right
                        rightMargin: units.gu(1)
                        top: parent.top
                    }
                    ListItem {
                        height: units.gu(5)
                        Item {
                            id: p
                            width: tl.width + units.gu(1)
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
                                    topMargin: units.gu(2)
                                    leftMargin: units.gu(1)
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
                                leftMargin: units.gu(1)
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
                                    PopupUtils.open("qrc:/qml/popovers/RecipientPopover.qml", delegate, {address: message.from})
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
        maxHeight: container.height - units.gu(2)
    }
}


import QtQuick 2.4
import Ubuntu.Components 1.3
import com.canonical.Oxide 1.9 as Oxide
import Dekko.Mail 1.0
import Dekko.Components 1.0
import "../../components"
import "../../webview"

DekkoPage {

    pageHeader.title: "Message"
    pageHeader.backAction: Action {
        id: back
        iconName: "back"
        onTriggered: internalStack.pop()
    }
    pageHeader.state: "custom"
    // only show the divider when the oxide moves the envelope header
    // when scrolling. We want it to look seemless in it's original position
    pageHeader.showDivider: h.y !== 0
    pageHeader.customComponent: Component {
        id: headerComponent
        Item {
            anchors.fill: parent
            anchors {
                topMargin: units.gu(1)
            }


            Item {
                height: parent.height
                anchors {
                    left: parent.left
                    right: parent.right
                }
                Row {
                    height: parent.height
                    anchors {
                        left: parent.left
                        right: starIcon.left
                    }
                    spacing: dekko.viewState.isSmallFF ? 0 : units.gu(1)

                    AbstractButton {
                        id: backBtn
                        height: parent.height
                        width: dekko.viewState.isSmallFF ? units.gu(4) : 0
                        Icon {
                            height: units.gu(2)
                            width: units.gu(2)
                            visible: dekko.viewState.isSmallFF
                            name: "back"
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                                top: parent.top
                                topMargin: units.gu(1.5)
                            }
                        }
                        onClicked: internalStack.pop()
                    }

                    AbstractButton {
                        height: parent.height
                        width: units.gu(4)
                        CachedImage {
                            height: units.gu(2.5)
                            width: units.gu(2.5)
                            name: Icons.MailReadIcon
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                                top: parent.top
                                topMargin: units.gu(1.2)
                            }
                        }
                    }

                    AbstractButton {
                        height: parent.height
                        width: units.gu(4)
                        CachedImage {
                            height: units.gu(2.5)
                            width: units.gu(2.5)
                            name: Icons.ViewListSymbolic
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                                top: parent.top
                                topMargin: units.gu(1.2)
                            }
                        }
                    }
                    AbstractButton {
                        height: parent.height
                        width: units.gu(4)
                        CachedImage {
                            height: units.gu(2.5)
                            width: units.gu(2.5)
                            name: Icons.JunkIcon
                            color: message.isJunk ? UbuntuColors.red : UbuntuColors.ash
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                                top: parent.top
                                topMargin: units.gu(1.2)
                            }
                        }
                    }
                }

                AbstractButton {
                    id: starIcon
                    height: parent.height
                    width: units.gu(4)
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }
                    CachedImage {
                        height: units.gu(2.5)
                        width: units.gu(2.5)
                        name: message && message.isImportant ? Icons.StarredIcon : Icons.UnStarredIcon
                        color: message && message.isImportant ? "#f0e442" : "#888888"
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            top: parent.top
                            topMargin: units.gu(1.2)
                        }
                    }
                    onClicked: Client.markMessageImportant(message.messageId, !message.isImportant)
                }

                Row {
                    height: parent.height
                    anchors {
                        left: starIcon.right
                        right: parent.right
                    }
                    layoutDirection: Qt.RightToLeft
                    spacing: dekko.viewState.isSmallFF ? 0 : units.gu(1)

                    AbstractButton {
                        id: delBtn
                        height: parent.height
                        width: units.gu(4)
                        CachedImage {
                            height: units.gu(2.5)
                            width: units.gu(2.5)
                            name: Icons.ContextMenuIcon
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                                top: parent.top
                                topMargin: units.gu(1.2)
                            }
                        }
                    }

                    AbstractButton {
                        height: parent.height
                        width: units.gu(4)
                        CachedImage {
                            height: units.gu(2.5)
                            width: units.gu(2.5)
                            name: Icons.MailForwardIcon
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                                top: parent.top
                                topMargin: units.gu(1.2)
                            }
                        }
                    }
                    AbstractButton {
                        height: parent.height
                        width: units.gu(4)
                        CachedImage {
                            height: units.gu(2.5)
                            width: units.gu(2.5)
                            name: Icons.MailReplyAllIcon
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                                top: parent.top
                                topMargin: units.gu(1.2)
                            }
                        }
                    }
                    AbstractButton {
                        height: parent.height
                        width: units.gu(4)
                        CachedImage {
                            height: units.gu(2.5)
                            width: units.gu(2.5)
                            name: Icons.MailReplyIcon
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                                top: parent.top
                                topMargin: units.gu(1.2)
                            }
                        }
                    }
                }
            }
        }
    }

    property alias msgId: message.messageId
    Message {
        id: message
        onBodyChanged: {
            console.log("MainPartUrl: ", body)
            webview.setCidQuery(message.messageId)
            webview.setBodyUrl(body)
        }
    }

    Item {
        clip: true
        anchors {
            left: parent.left
            right: parent.right
            top: pageHeader.bottom
            bottom: parent.bottom
        }
        DekkoWebView {
            id: webview
            anchors {
                fill: parent
                leftMargin: !dekko.viewState.isSmallFF ? units.gu(1) : 0
            }

            url: "about:blank"
            locationBarController.mode: Oxide.LocationBarController.ModeAuto
            locationBarController.height: h.height
        }
        MessageHeader {
            id: h
            msg: message
            width: parent.width
            y: webview ? webview.locationBarController.offset : 0
        }
    }
}


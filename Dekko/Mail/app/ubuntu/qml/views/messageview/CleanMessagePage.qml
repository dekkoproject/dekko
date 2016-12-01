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
import com.canonical.Oxide 1.9 as Oxide
import Dekko.Mail 1.0
import Dekko.Components 1.0
import "../../components"
import "../../webview"
import "../../constants"

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
                    spacing: dekko.isSmallFF ? 0 : Style.smallSpacing

                    AbstractButton {
                        id: backBtn
                        height: parent.height
                        width: dekko.isSmallFF ? Style.largeSpacing : 0
                        Icon {
                            height: Style.defaultSpacing
                            width: Style.defaultSpacing
                            visible: dekko.isSmallFF
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
                            height: Style.defaultIconSize
                            width: Style.defaultIconSize
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
                            height: Style.defaultIconSize
                            width: Style.defaultIconSize
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
                            height: Style.defaultIconSize
                            width: Style.defaultIconSize
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
                        height: Style.defaultIconSize
                        width: Style.defaultIconSize
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
                    spacing: dekko.isSmallFF ? 0 : Style.smallSpacing

                    AbstractButton {
                        id: delBtn
                        height: parent.height
                        width: units.gu(4)
                        CachedImage {
                            height: Style.defaultIconSize
                            width: Style.defaultIconSize
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
                            height: Style.defaultIconSize
                            width: Style.defaultIconSize
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
                            height: Style.defaultIconSize
                            width: Style.defaultIconSize
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
                            height: Style.defaultIconSize
                            width: Style.defaultIconSize
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
                leftMargin: !dekko.isSmallFF ? units.gu(1) : 0
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


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
import Dekko.Components 1.0
import Dekko.Settings 1.0
import QuickFlux 1.0
import "../../actions"
import "../../actions/logging"
import "../../actions/messaging"
import "../../actions/views"
import "../../actions/contacts"
import "../../stores"
import "../../stores/contacts"
import "../components"
import "../../constants"

BaseStage {
    id: ms

    panelEnabled: devModeEnabled
    
    // We use a stretch row here rather than RowLayout
    // Just because we can use the implicit size hints to stretch the correct
    // panel. Yes we could use Layout.fillWidth but in the future there maybe
    // more columns added to this (through plugins) and we may want to share remaining width evenly between two
    // or more colums. Which StretchRow handles nicely
    StretchRow {
        spacing: 0
        anchors.fill: parent
        // Should only be visible on large FF
        // Access is done via the navigation drawer
        // for smaller FF's
        PanelContainer {
            visible: dekko.viewState.isLargeFF
            resizable: !dekko.viewState.isSmallFF
            minSize: units.gu(30)
            maxSize: units.gu(50)
            height: parent.height
            activeEdge: Item.Right
            StageArea {
                id: navMenuStage
                stageID: ViewKeys.navigationStack
                anchors.fill: parent
                baseUrl: "qrc:/qml/views/NavMenuPage.qml"
            }
        }

        PanelContainer {
            // This is where we could no longer
            // work with AdaptivePageLayout
            // Our center column is actually our main page
            // when on small form factor devices, and having to
            // pop/push and rejig evenrything was just awful
            // Plus i prefer our api.
            // So we set this main page to fill screen width when
            // on small FF. This sets the implicit width to -1
            // and restores it on going back to larger FF's
            stretchOnSmallFF: true
            resizable: !dekko.viewState.isSmallFF
            minSize: units.gu(40)
            maxSize: units.gu(60)
            size: units.gu(40)
            height: parent.height
            activeEdge: Item.Right
            StageArea {
                id: msgListStage
                stageID: ViewKeys.messageListStack
                anchors.fill: parent
                baseUrl: "qrc:/qml/views/MessageListView.qml"
                function rewind() {
                    var needsDelay = false
                    if (stackCount > 1) {
                        needsDelay = true
                        while (stackCount !== 1) {
                            pop()
                        }
                    }
                    if (needsDelay) {
                        delaySignalRewind.start()
                    } else {
                        MessageActions.stackRewound()
                    }
                }
            }
        }
        // Take rest of space when visible
        Stretcher {
            visible: !dekko.viewState.isSmallFF
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            StageArea {
                id: msgViewStage
                stageID: ViewKeys.messageViewStack
                anchors.fill: parent
                immediatePush: false
                baseUrl: "qrc:/qml/views/NothingSelectedPage.qml"
                delegate: DekkoAnimation.customStackViewDelegate1
                AppListener {
                    filter: MessageKeys.openFolder
                    onDispatched: {
                        if (msgViewStage.stackCount > 1) {
                            msgViewStage.pop()
                        }
                    }
                }
            }
        }
    }

    GlobalSettings {
        id: globalSettings
        function messageViewStyle() {
            if (data.messageview.style === "default") {
                return "qrc:/qml/views/messageview/DefaultMessagePage.qml"
            } else if (data.messageview.style === "clean") {
                return "qrc:/qml/views/messageview/CleanMessagePage.qml"
            }
        }
    }

    Timer {
        id: delaySignalRewind
        interval: 300
        repeat: false
        onTriggered: MessageActions.stackRewound()
    }

    AppListener {
        Filter {
            type: MessageKeys.openMessage
            onDispatched: {
                var style = globalSettings.messageViewStyle()
                if (dekko.viewState.isSmallFF) {
                    // leftStage push msgview
                    ViewActions.pushToStageArea(ViewKeys.messageListStack, style, {msgId: message.msgId})
                } else {
                    if (msgViewStage.stackCount > 1) {
                        ViewActions.replaceTopStageAreaItem(ViewKeys.messageViewStack, style, {msgId: message.msgId})
                    } else {
                        ViewActions.pushToStageArea(ViewKeys.messageViewStack, style, {msgId: message.msgId})
                    }
                }
            }
        }

        Filter {
            type: MessageKeys.rewindMessageListStack
            onDispatched: {
                // Listen for a new folder opening and pop the stack
                // until we get to the msglist. MailStore is also listening on this and will take
                // care of actually opening the folder.
                Log.logInfo("MailStage::openFolder", "Checking stack count")
                msgListStage.rewind()
            }
        }

        Filter {
            type: MessageKeys.openAccountFolder
            onDispatched: {
                if (dekko.viewState.isLargeFF) {
                    ViewActions.pushToStageArea(ViewKeys.navigationStack,
                                                "qrc:/qml/views/FolderListView.qml",
                                                {
                                                    pageTitle: message.accountName,
                                                    accountId: message.accountId
                                                })
                } else {
                    ViewActions.pushToStageArea(ViewKeys.messageListStack,
                                                "qrc:/qml/views/FolderListView.qml",
                                                {
                                                    pageTitle: message.accountName,
                                                    accountId: message.accountId
                                                })
                }
            }
        }
    }

    AppListener {
        Filter {
            type: ContactKeys.openAddressBook
            onDispatched: {
                ViewActions.pushToStageArea(ViewKeys.messageListStack,
                                            ab,
                                            {
                                                pageTitle: message.accountName,
                                                accountId: message.accountId
                                            })
            }
        }
    }
}


/* Copyright (C) 2014-2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu Devices/

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
import QtFeedback 5.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import com.canonical.Oxide 1.12 as Oxide
import Dekko.Components 1.0
import "../components"
import "./actions"

Oxide.WebView {
    id: webview

    property bool interestingIsHtml: true
    property bool fullScreen: false
    property int messageUid

    function setCidQuery(uid) {
        messageUid = uid
        ctxt.setQuery()
    }

    function setBodyUrl(body) {
        ctxt.setQuery()
        webview.url = body
    }

    function zoom(val) {
        var level = {}
        level["level"] = "%1%".arg(val)
        webview.rootFrame.sendMessageNoReply(defaultContext, "ZOOM_LEVEL", level);
    }

    // LOCK IT DOWN!!
    preferences {
        // We should NEVER allow javascript to run in
        // a message body. See https://miki.it/blog/2013/9/24/mailboxapp-javascript-execution/ for
        // how mailbox (by dropbox) got it wrong :-)
        javascriptEnabled: false
        javascriptCanAccessClipboard: false
        allowUniversalAccessFromFileUrls: false
        loadsImagesAutomatically: true
        allowScriptsToCloseWindows: false
        appCacheEnabled: true
        localStorageEnabled: false
        defaultEncoding: "UTF-8"
        fixedFontFamily: "Ubuntu"
        serifFontFamily: "Ubuntu"
        sanSerifFontFamily: "Ubuntu"
    }

    context: DekkoWebContext {
        id: ctxt
        messageUid: webview.messageUid
    }

    Item {
        id: contextualRectangle
        visible: false
        readonly property real locationBarOffset: webview.locationBarController.height + webview.locationBarController.offset
        x: internal.ctxtModel ? internal.ctxtModel.position.x : 0
        y: internal.ctxtModel ? internal.ctxtModel.position.y + locationBarOffset : 0
    }

    contextMenu: Popover {
        id: actionPopover
        z: 5 // Prevent being rendered below the message header
        property var msg
        caller: contextualRectangle

        Column {
            id: containerLayout

            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }

            ContextGroup {
                contextActions: [
                    ContextAction {
                        description: qsTr("Open in browser")
                        actionIcon: Icons.PasteIcon
                    },
                    ContextAction {
                        description: qsTr("Copy link")
                        actionIcon: Icons.CopyIcon
                        visible: internal.ctxtModel && internal.ctxtModel.linkUrl.toString()
                        onTriggered: Clipboard.push(["text/plain", internal.ctxtModel.linkUrl.toString()])
                    },
                    ContextAction {
                        description: qsTr("Share link")
                        actionIcon: Icons.ShareIcon
                    }

                ]
            }

            ContextGroup {
                contextActions: [
                    ContextAction {
                        description: qsTr("Reply")
                        actionIcon: Icons.PasteIcon
                    },
                    ContextAction {
                        description: qsTr("Reply all")
                        actionIcon: Icons.CopyIcon
                        visible: internal.ctxtModel && internal.ctxtModel.linkUrl.toString()
                        onTriggered: Clipboard.push(["text/plain", internal.ctxtModel.linkUrl.toString()])
                    },
                    ContextAction {
                        description: qsTr("Forward")
                        actionIcon: Icons.ShareIcon
                    }

                ]
            }
            ContextGroup {
                contextActions: [
                    ContextAction {
                        description: qsTr("View source")
                        actionIcon: Icons.PasteIcon
                    }
                ]
            }
        }
        // Override default implementation to prevent context menu from stealing
        // active focus when shown (https://launchpad.net/bugs/1526884).
        function show() {
            visible = true
            __foreground.show()
        }

        Component.onCompleted: {
            internal.ctxtModel = model
            show()
        }
    }

    QtObject {
        id: internal
        property QtObject ctxtModel: null
    }

    onNavigationRequested: {
        console.log("[Dekko Web View] RequestUrl", request.url)
        clickEffect.start()
        PopupUtils.open(linkClickedDialog, dekko, {externalLink: request.url})
        request.action = 255
    }

    onJavaScriptConsoleMessage: {
        var msg = "[Dekko Web View] [JS] (%1:%2) %3".arg(sourceId).arg(lineNumber).arg(message)
        console.log(msg)
    }

    HapticsEffect {
        id: clickEffect
        attackIntensity: 0.0
        attackTime: 50
        intensity: 1.0
        duration: 10
        fadeTime: 50
        fadeIntensity: 0.0
    }

    Component {
        id: linkClickedDialog
        Item {}
//        ConfirmationDialog {
//            id: linkDialog
//            property string externalLink
//            title: qsTr("Open in browser?")
//            text: qsTr("Confirm to open %1 in web browser").arg(externalLink.substring(0, 30))
//            onConfirmClicked: Qt.openUrlExternally(externalLink)
//        }
    }
}


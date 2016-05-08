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
import Dekko.Settings 1.0
import "../components"
import "../dialogs"

Oxide.WebView {
    id: webview

    property bool fullScreen: false
    property int messageUid
    readonly property alias contentBlocked: internal.remoteContentBlockRunning

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

    function allowBlockedContent() {
        internal.remoteContentBlockRunning = false
        ctxt.setRemoteAllowedForThisMessage(true)
        reload()
    }

    GlobalSettings{
        id: globalSettings
    }

    // LOCK IT DOWN!!
//    incognito: true can't set his as it disables userscripts.
    preferences {
        // We should NEVER allow javascript to run in
        // a message body. See https://miki.it/blog/2013/9/24/mailboxapp-javascript-execution/ for
        // how mailbox (by dropbox) got it wrong :-)
        javascriptEnabled: false
        javascriptCanAccessClipboard: false
        allowUniversalAccessFromFileUrls: false
//        loadsImagesAutomatically: globalSettings.get(GlobalSettings.AutoLoadImages)
        allowScriptsToCloseWindows: false
        appCacheEnabled: false
        localStorageEnabled: false
        hyperlinkAuditingEnabled: false
        defaultEncoding: "UTF-8"
        fixedFontFamily: globalSettings.data && globalSettings.data.messageview.useMonospaceFont ? "Ubuntu Monospace" : "Ubuntu"
        serifFontFamily: "Ubuntu"
        sanSerifFontFamily: "Ubuntu"
    }

    context: DekkoWebContext {
        id: ctxt
        messageUid: webview.messageUid
        remoteContentAllowed: globalSettings.get(GlobalSettings.AllowRemoteContent)
        onRemoteContentBlocked: {
            if (!internal.remoteContentBlockRunning) {
                internal.remoteContentBlockRunning = true
            }
        }
    }

    // Binding the contxt popover directly to this causes
    // it to get rendered below message header.
    // So instead just use this item to grab the model context property
    // And popup the dialog outside of this binding.
    contextMenu: Item {
        Component.onCompleted: {
            internal.ctxtModel = model
            PopupUtils.open("qrc:/qml/views/popovers/MessageViewContextMenu.qml", contextualRectangle, {ctxtModel: internal.ctxtModel})
        }
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

    Item {
        id: contextualRectangle
        visible: false
        readonly property real locationBarOffset: webview.locationBarController.height + webview.locationBarController.offset
        x: internal.ctxtModel ? internal.ctxtModel.position.x : 0
        y: internal.ctxtModel ? internal.ctxtModel.position.y + locationBarOffset : 0
        z: 50
    }

    QtObject {
        id: internal
        property QtObject ctxtModel: null
        property bool remoteContentBlockRunning: false
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
        ConfirmationDialog {
            id: linkDialog
            property string externalLink
            title: qsTr("Open in browser?")
            text: qsTr("Confirm to open %1 in web browser").arg(externalLink.substring(0, 30))
            onConfirmClicked: Qt.openUrlExternally(externalLink)
        }
    }
}


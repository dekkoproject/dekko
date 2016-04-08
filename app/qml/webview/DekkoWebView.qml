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
import Ubuntu.Components.Popups 1.0
import com.canonical.Oxide 1.9
import Dekko.Components 1.0

WebView {
    id: webview

    property bool interestingIsHtml: true
    property bool fullScreen: false
    property int messageUid

    readonly property string defaultContext: "dekko://"

    function setCidQuery(uid) {
        messageUid = uid
        networkDelegate.setQuery()
    }

    function setBodyUrl(body) {
        networkDelegate.setQuery()
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

    context: WebContext {
//        devtoolsEnabled: true
//        devtoolsPort: 9232
        // These are our internal schemes. For the html parts inside
        // the sandboxed iframe we will be able to set the src attribute to our
        // dekko-msg(part)://msg/0/1 scheme and it will fetch that part :-) no need to wait on
        // manual fetching here \o/
        allowedExtraUrlSchemes: ["dekko-msg", "dekko-part", "cid"]
        popupBlockerEnabled: true
        userScripts: [
//            UserScript {
//                context: "dekko://"
//                url: Qt.resolvedUrl("./userscripts/contact-message.js")
//                matchAllFrames: true
//            },
            UserScript {
                context: defaultContext
                url: Paths.userscript(Paths.FontScript)
                matchAllFrames: true
            },
            UserScript {
                context: defaultContext
                url: Paths.userscript(Paths.ViewportScript)
                matchAllFrames: true
            },
            UserScript {
                context: defaultContext
                url: Paths.userscript(Paths.ZoomScript)
                matchAllFrames: true
            }/*,
            UserScript {
                context: "dekko://"
                url: fullScreen ? Qt.resolvedUrl("./userscripts/fake_script.js") : Qt.resolvedUrl("./userscripts/hide_overflow.js")
                matchAllFrames: true
            }*/
        ]
        networkRequestDelegate: WebContextDelegateWorker {
            id: networkDelegate
            /* Because we are potentially in a thread view, we may have multiple instances of oxide
               so we need to modify any of our internal cid:* schemes to figure out which message the url belongs
               to. This get's done on the redirectUrl as we can't modify the initial url directly.

               THe custom QNAM will return a ForbiddenReply which will trigger the redirection with our correctly
               formatted url query

               see: userscripts/add_cid_query.js
             */
            property string cidQuery: "?messageId=%1".arg(messageUid)

            function setQuery() {
                console.log("CID QUERY IS: ", cidQuery)
                sendMessage({query: cidQuery})
            }

            source: Paths.userscript(Paths.CidQueryScript)
            onMessage: {
//                console.log("NetworkRequest url: ", message.url)
//                console.log("NetworkRequest query: ", message.query)
            }
            Component.onCompleted: {
                setQuery()
            }
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


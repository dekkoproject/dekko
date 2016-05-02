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
import com.canonical.Oxide 1.12 as Oxide
import Dekko.Components 1.0

Oxide.WebContext {
    id: ctxt

    property int messageUid
    readonly property string defaultContext: "dekko://"
    property bool remoteContentAllowed: false

    signal remoteContentBlocked()

    function setQuery() { networkDelegate.setQuery() }

    function setRemoteAllowedForThisMessage(allowed) {
        networkDelegate.setBlockRemoteResources(!allowed)
    }

    userAgent: _ua.defaultUA
    /* These are our supported custom url schemes

      dekko-msg: is for non multipart messages that typically only contain either a text/plain or text/html part
                 This scheme get's routed to a MsgReply instance that downloads the complete message and returns
                 the body part

      dekko-part: is for multipart messages and points to the interesting text/plain or text/html part that is the main body
                  This scheme get's routed to a MsgPartReply instance that downloads only the referenced par and returns it

      cid: is for inline message parts that are referenced in the loaded dekko-part/dekko-msg.
           This scheme get's translated into a valid dekko-part and handled via MsgPartReply same as above.
           This scheme requires that the messageUid is added as a query to the url (see network delegate below)
    */
    allowedExtraUrlSchemes: ["dekko-msg", "dekko-part", "cid"]
    // No popups allowed
    popupBlockerEnabled: true
    doNotTrackEnabled: true
    userScripts: [
        Oxide.UserScript {
            context: defaultContext
            url: Paths.userscript(Paths.FontScript)
            matchAllFrames: true
        },
        Oxide.UserScript {
            context: defaultContext
            url: Paths.userscript(Paths.ViewportScript)
            matchAllFrames: true
        },
        Oxide.UserScript {
            context: defaultContext
            url: Paths.userscript(Paths.ZoomScript)
            matchAllFrames: true
        }
    ]
    networkRequestDelegate: Oxide.WebContextDelegateWorker {
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
            //                console.log("CID QUERY IS: ", cidQuery)
            sendMessage({type: "QUERY", value: cidQuery })
            setBlockRemoteResources(!remoteContentAllowed)
        }

        function setBlockRemoteResources(block) {
            sendMessage({type: "REMOTE_ACCESS", value: block})
        }

        source: Paths.userscript(Paths.CidQueryScript)
        onMessage: {
            switch (message.type) {
            case "DEBUG":
                console.log("DEBUG: Url is: ", message.url);
                break;
            case "QUERY":
                console.log("Message query: ", message.value)
                break;
            case "REMOTE_ACCESS":
//                console.log("Remote resource blocked!", message.url)
                ctxt.remoteContentBlocked()
                break;
            }
        }
        Component.onCompleted: {
            setQuery()
        }
    }

    /* Stolen from Ubuntu.Web.UbuntuWebContext
     * Useful documentation:
     *   http://en.wikipedia.org/wiki/User_agent#Format
     *   https://developer.mozilla.org/en-US/docs/Gecko_user_agent_string_reference
     *   https://wiki.mozilla.org/B2G/User_Agent
     *   https://github.com/mozilla-b2g/gaia/blob/master/build/ua-override-prefs.js
     *   https://developers.google.com/chrome/mobile/docs/user-agent
     */

    property QtObject _ua: QtObject {
        id: defaultUA
        // Empirical value: screens smaller than 19cm are considered small enough that a
        // mobile UA string is used, screens bigger than that will get desktop content.
        readonly property bool smallScreen: ctxt.screenDiagonal < 190

        // %1: Ubuntu version, e.g. "14.04"
        // %2: optional token to specify further attributes of the platform, e.g. "like Android"
        // %3: optional hardware ID token
        // %4: WebKit version, e.g. "537.36"
        // %5: Chromium version, e.g. "35.0.1870.2"
        // %6: Optional token to provide additional free-form information, e.g. "Mobile"
        // %7: Safari version, e.g. "537.36"
        // %8: Optional token, in case some extra bits are needed to make things work (e.g. an extra form factor info etc.)
        //
        // note #1: "Mozilla/5.0" is misinformation, but it is a legacy token that
        //   virtually every single UA out there has, it seems unwise to remove it
        // note #2: "AppleWebKit", as opposed to plain "WebKit", does make a
        //   difference in the content served by certain sites (e.g. gmail.com)
        readonly property string _template: "Mozilla/5.0 (Linux; Ubuntu %1%2%3) AppleWebKit/%4 Chromium/%5 %6Safari/%7%8"

        // FIXME: compute at build time (using lsb_release)
        readonly property string _ubuntuVersion: "15.04"

        readonly property string _attributes: smallScreen ? "like Android 4.4" : ""

        readonly property string _hardwareID: ""

        // See chromium/src/content/webkit_version.h.in in oxide’s source tree.
        readonly property string _webkitVersion: "537.36"

        // See chromium/src/chrome/VERSION in oxide’s source tree.
        // Note: the actual version number probably doesn’t matter that much,
        //       however its format does, so we probably don’t need to bump it
        //       every time we rebase on a newer chromium.
        readonly property string _chromiumVersion: "35.0.1870.2"

        readonly property string _formFactor: smallScreen ? "Mobile" : ""

        readonly property string _more: ""

        property string defaultUA: {
            var ua = _template
            ua = ua.arg(_ubuntuVersion) // %1
            ua = ua.arg((_attributes !== "") ? " %1".arg(_attributes) : "") // %2
            ua = ua.arg((_hardwareID !== "") ? "; %1".arg(_hardwareID) : "") // %3
            ua = ua.arg(_webkitVersion) // %4
            ua = ua.arg(_chromiumVersion) // %5
            ua = ua.arg((_formFactor !== "") ? "%1 ".arg(_formFactor) : "") // %6
            ua = ua.arg(_webkitVersion) // %7
            ua = ua.arg((_more !== "") ? " %1".arg(_more) : "") // %8
            return ua
        }
    }
}

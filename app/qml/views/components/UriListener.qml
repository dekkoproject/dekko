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
import QuickFlux 1.0
import "../../actions/composer"
import "../../actions/logging"
import "../../actions/views"
import "../../stores/composer"
import "../utils/QtCoreAPI.js" as QtCoreAPI

AppListener {

    waitFor: [ComposerStore.listenerId]

    Filter {
        type: ViewKeys.openUri
        onDispatched: {
            d.processUri(message.uri)
        }
    }

    Connections {
        target: UriHandler
        onOpened: {
            Log.logInfo("UriListener::UriHandler::onOpened", "We got uris to handle: " + uris)
            d.processUris(uris)
        }
    }

    // We make these private as we want to enforce the use
    // of the dispatcher api.
    // If you need to open a uri from anywhere in the app
    // then you just need to dispatch ViewActions.openUri
    QtObject {
        id: d

        function processUris(uris) {
            if (!uris.isEmpty()) {
                uris.foreach(function (idx, uri) {
                    processUri(uri);
                })
            }
        }

        function processUri(uri) {
            Log.logInfo("UriListener::processUri", "Processing URI: " + uri)
            if (typeof uri === "undefined") {
                Log.logInfo("UriListener::processUri", "false alarm.. its a bum uri")
                return
            }
            if (uri.startsWith("mailto:")) {
                Log.logInfo("UriListener::processUri", "Recieved mailto: uri. Opening composer")
                ComposerActions.composeMailtoUri(uri)
                return
            }

            if (uri.startsWith("dekko:")) {
                var commands = uri.split("://")[1].split("/")
                if (commands.isEmpty()) {
                    Log.logInfo("UriListener::processUri", "Dekko command is empty, nothing we can do")
                    return
                }
                switch (commands.at(0).toLowerCase()) {
                case "notify":
                    Log.logInfo("UriListener::processUri", "Were opening a notification")
                    break
                case "contacts":
                    Log.logInfo("UriListener::processUri", "We're opening contacts")
                    switch (commands.at(1)) {
                    case "new":
                        Log.logInfo("UriListener::processUri", "Create new contact")
                        break
                    case "edit":
                        Log.logInfo("UriListener::processUri", "Edit existing contact")
                        break
                    case "remove":
                        Log.logInfo("UriListener::processUri", "Remove contact")
                        break
                    }
                    break
                case "settings":
                    Log.logInfo("UriListener::processUri", "We're opening settings")
                    break
                }
                return
            }
            if (uri.startsWith("http://") || uri.startsWith("https://")) {
                Log.logInfo("UriListener::processUri", "Recieved external uri. Prompting user to allow opening externally")
                return
            }
        }
    }

    Component.onCompleted: {
        if (appUris) {
            Log.logInfo("UriListener::onCompleted", "We got some uri's to handle: " + appUris)
            d.processUris(appUris)
        }
    }
}

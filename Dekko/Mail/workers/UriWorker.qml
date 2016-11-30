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
import QuickFlux 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Composer 1.0
import Dekko.Utils 1.0 as JSUtils

AppListener {

    waitFor: [ComposerStore.listenerId]

    Filter {
        type: ViewKeys.openUri
        onDispatched: {
            d.processUri(message.uri)
        }
    }
//      TODO: Move this to where the worker is declared and call ViewKeys.openUris()
//    Connections {
//        target: UriHandler
//        onOpened: {
//            Log.logInfo("UriListener::UriHandler::onOpened", "We got uris to handle: " + uris)
//            d.processUris(uris)
//        }
//    }

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
                    if (commands.size() < 2) {
                        Log.logInfo("UriListener::processUri", "Not enough args to show a notification")
                        return
                    } else if (commands.size() === 2) {
                        Log.logInfo("UriListener::processUri", "Navigating to account")
                        // Call later: This is equivalent to ViewActions.navigateToAccount(commands.at(1))
                        ViewActions.delayCallWithArgs(ViewKeys.navigateToAccount, {accountId: commands.at(1)})
                    } else if (commands.size() === 3) {
                        Log.logInfo("UriListener::processUri", "Navigating to folder")
                        // Call later: ViewActions.navigateToFolder(commands.at(1), commands.at(2))
                        ViewActions.delayCallWithArgs(ViewKeys.navigateToFolder,
                                                      {
                                                          accountId: commands.at(1),
                                                          folderId: commands.at(2)
                                                      })
                    } else if (commands.size() === 4) {
                        Log.logInfo("UriListener::processUri", "Navigating to message")
                        // Call later: ViewActions.navigateToMessage(commands.at(1), commands.at(2), commands.at(3))
                        ViewActions.delayCallWithArgs(ViewKeys.navigateToMessage,
                                                      {
                                                          accountId: commands.at(1),
                                                          folderId: commands.at(2),
                                                          messageId: commands.at(3)
                                                      })
                    }
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
                    case "open":
                        Log.logInfo("UriListener::processUri", "Opening addressbook")
                        // We have most likely got this off the commandline args or from url dispatcher
                        // now we need to wait for the stage stacks to be done first so just
                        // schedule a dispatch in a little while.
                        ViewActions.delayCall(ViewKeys.openAddressBook)
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

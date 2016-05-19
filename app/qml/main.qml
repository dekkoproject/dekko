/* Copyright (C) 2014-2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu Touch/

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
import "./views/components"
import "./views/dialogs"

Dekko {
    id: dekko
    anchors.fill: parent

    // Add these components to the resources list
    QtObject {
        id: d
        property DialogQueue dlgQueue: DialogQueue {}
        property Logger logger: Logger {
            devLoggingEnabled: devModeEnabled
        }
    }
}

//    Connections {
//        target: ContentHub
//        onImportRequested: {
//            console.log("[ContentHub::importRequested] [STATUS] import requested, running import...")
//            importTransfer = transfer;
//            handleContentHubImport()
//        }
//        onShareRequested: {
//            console.log("[ContentHub::shareRequested] [STATUS] share requested. importing share")
//            importTransfer = transfer;
//            handleContentHubImport()
//        }
//        onExportRequested: {
//            console.log("ContentHub::exportRequested: NotImplementedYet")
//        }
//    }

//    property var uri: undefined

//    Connections {
//        target: UriHandler
//        onOpened: {
//            dekko.uri = uris[0];
//            console.log("[UriHandler::opened] [INFO] uri opened: " + dekko.uri);
//            processUri();
//        }
//    }

//    function processUri() {
//        if (typeof dekko.uri === "undefined") {
//            return;
//        }

//        var mailto = dekko.uri.split(":")[0];
//        if (mailto === "mailto") {
//            console.log("OPENING MAILTO: ", dekko.uri)
//            openMailTo(dekko.uri)
//            return
//        }
//        var commands = dekko.uri.split("://")[1].split("/")
//        if (commands) {
//            switch(commands[0].toLowerCase()) {
//            case "notify":
//                console.log("notify received")
//                rootPageStack.clear()
//                accountsManager.openMailbox(commands[1], commands[2]);
//                rootPageStack.push(Qt.resolvedUrl("./MessageListView/MessageListPage.qml"))
//                break;
//            }
//        }
//    }

//    Component.onCompleted: {
//        if (appUris) {
//            console.log("WE HAVE A URIs: ", appUris)
//            dekko.uri = appUris.substr(1, appUris.length - 2)
//            console.log("DEKKO URI: ", dekko.uri)
//            processUri()
//        }
//        if (mailToArgs) {
//            console.log("MailToArgs:", mailToArgs)
//            dekko.uri = mailToArgs[0]
//            if (dekko.uri) {
//                processUri()
//            }
//        }
//    }


//}

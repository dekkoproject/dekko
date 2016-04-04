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
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0 as ListItems
import Ubuntu.PushNotifications 0.1
import Ubuntu.Content 1.0
import Dekko.Controls 1.0
import Dekko.Settings 1.0
import "./components"
import "./utils"

// The MainView doesn't provide us any benefit here
// the only thing we are interested in is the anchorToKeyboard stuff
// This Item contains a nested Item container that contains the main stage which
// get's anchored to the KeyboardRectangle component to provide
// the same functionality as the MainView anchorToKbd prop
Item {
    id: dekko
    //-----------------------------------
    // SIGNALS
    //-----------------------------------

    //----------------------------------
    // SLOTS
    //----------------------------------

    //----------------------------------
    // OBJECT PROPERTIES
    //----------------------------------
    objectName: "dekko" // object name for functional testing
    //    applicationName: "dekko.dekkoproject"
    //    automaticOrientation: false
    //    anchorToKeyboard: true // ensure page doesn't end up behind the OSK
    // Random values to simulate a phone sized window on desktop
    // THis get's overriden on device
    width: units.gu(120); height: units.gu(80)

    //--------------------------------
    // FUNCTIONS
    //--------------------------------


    //--------------------------------
    // ACTIONS
    //--------------------------------

    //--------------------------------
    // PRIVATE PROPERTIES & FUNCTIONS
    //--------------------------------
    property alias viewState: view

    ViewState {
        id: view
        anchors.fill: parent
        onStateChanged: console.log("[ViewState::stateChanged] [STATUS] view state is now: ", state)
    }

    QueuedApplicationAlertManager {
        id: alertQueue
//        accountsManager: dekko.accountsManager
    }

    function showNotice(notice) {
        popupQueue.queue("qrc:/qml/popovers/NoticePopup.qml", dekko, {title: "Notice", text: notice})
    }

    PopupQueue {
        id: popupQueue
    }

    Item {
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
            bottom: kbdRect.top
        }
        PageStack {
            id: rootPageStack
            Component.onCompleted: push("qrc:/qml/stages/MainStage.qml")
        }
    }

    KeyboardRectangle {
        id: kbdRect
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


}

//-----------------------------------
// PROPERTIES
//-----------------------------------
// This property holds the currently selected account
// and should be used throughout all other views
// e.g For a listview set the model to dekko.currentAccount.imapModel
//    property alias currentAccount: accountsManager.currentAccount
//    property bool msgListPushed: false

//    property var mailboxListPage;
//    // So we can access it further into the rootPageStack
//    property alias accountsManager: accountsManager

//    property bool networkAvailable: dekko.currentAccount.imapModel.isNetworkAvailable
//    property var submissionManager;

//    property var addressbookModel: addressbookModelsLoader.item.addressbookModel
//    property var addressbookModelFiltered: addressbookModelsLoader.item.addressbookModelFiltered
//    property var recentContactModel: addressbookModelsLoader.item.recentContactModel
//    property var recentContactModelFiltered: addressbookModelsLoader.item.recentContactModelFiltered
//    property var combinedFlattenContactFilterModel: addressbookModelsLoader.item.combinedFlattenContactFilterModel
//    property var flattenContactModel: addressbookModelsLoader.item.flattenContactModel
//    property var flattenRecentContactModel: addressbookModelsLoader.item.flattenRecentContactModel
//    property var combinedFlattenContactModel: addressbookModelsLoader.item.combinedFlattenContactModel
//    property var imapLogDrawer: imapLogDrawerLoader.item
//    property alias viewState: view
//    property bool setupWizardRunning: false
//    Rectangle {
//        anchors.fill: parent
//        color: Style.common.background
//    }
//    AccountsManager {
//        id: accountsManager
//        onImapError: {
//            console.log("[AccountsManager::onAlertReceived] [ERROR] IMAP error with message: ", error.error)
//            alertQueue.showImapError(error)
//        }
//        // FIXME: we are to display a non invasive inline status for this that doesn't prevent app
//        // interaction
//        // see: https://docs.google.com/a/canonical.com/presentation/d/1woHjO8K4iqyVZZlfQ4BXL0DhYbwkEmZ7wvcUhYzHDRk/edit#slide=id.g183c60488_08 for example
//        //        onNetworkError: alertQueue.showNetworkError(error)
//        onAlertReceived: {
//            console.log("[AccountsManager::onAlertReceived] [ALERT] IMAP alert recieved with message: ", message.message)
//            console.debug()
//            alertQueue.showImapAlert(message)
//        }
//        onAuthRequested: {
//            console.log("[AccountsManager::onAuthRequested] [STATUS] authentication requested, queuing password job")
//            alertQueue.authenticateRequested = true
//            passwordManager.requestPassword(account.accountId, PasswordManager.IMAP)
//        }
//        //        onConnectionStateChanged: console.log("STATE CHANGED")
//        onAuthAttemptFailed: {
//            console.log("[AccountsManager::onAuthAttemptFailed] [ERROR] Authentication Failed! :-( ")
//            console.log("[AccountsManager::onAuthAttemptFailed] [ERROR] Account: ", account.accountId)
//            console.log("[AccountsManager::onAuthAttemptFailed] [ERROR] AuthError: ", account.authError)
//            alertQueue.authAttemptFailed(account)
//        }
//        onCheckSslPolicy: {
//            console.log("CHECK SSL INFO CALLED")
//            alertQueue.showSslInfo(sslInfo)
//        }
//        onError: console.log("[AccountsManager::error] [ERROR] ", msg)
//        onCurrentAccountChanged: {
//            if (currentAccount !== null) {
//                console.log("Current Account Changed \o/ The id is: " + currentAccount.accountId)
//            } else {
//                console.log("[WARNING] Unloaded current account.... the next account that opens a connection \
//                            is going to steal the msglistview. Be sure this is what you want!!")
//            }
//        }
//        onNoAccountsConfigured: {
//            console.log("[AccountsManager::onNoAccountsConfigured] Running setup wizard.")
//            rootPageStack.push(Qt.resolvedUrl("./SetupWizard/SetupWizard.qml"), {hasAccounts: false})
//            setupWizardRunning = true;
//        }

//        onNewOnlineAccountCreated: {
//            console.log("[AccountsManager::onNewOnlineAccountCreated] NotImplementedYet")
//        }
//        onNewOnlineAccountEnabled: {
//            console.log("[AccountsManager::onNewOnlineAccountEnabled] Checking if SetupWizard is already running")
//            if (!setupWizardRunning) {
//                console.log("[AccountsManager::onNewOnlineAccountEnabled] SetupWizard not running. Launching now...")
//                rootPageStack.push(Qt.resolvedUrl("./SetupWizard/SetupWizard.qml"), {accountType: NewAccountType.ONLINE_ACCOUNT, oaId: id})
//                setupWizardRunning = true;
//            } else if (setupWizardRunning && count === 0) {
//                console.log("[AccountsManager::onNewOnlineAccountEnabled] SetupWizard is alread running, reloading to NewAccountType::ONLINE_ACCOUNT state")
//                rootPageStack.pop()
//                rootPageStack.push(Qt.resolvedUrl("./SetupWizard/SetupWizard.qml"), {accountType: NewAccountType.ONLINE_ACCOUNT, oaId: id})
//            }
//        }

//        Component.onCompleted: {
//            console.log("[AccountsManager::onCompleted] Opening accounts")
//            accountsManager.run()
//            console.log("[AccountsManager::onCompleted] Setting accounts manager on QNAM factory")
//            msgPartNetAccessManagerFactory.setAccountsManager(accountsManager)
//        }
//    }
//    Loader {
//        id: addressbookModelsLoader
//        asynchronous: true
//        source: Qt.resolvedUrl("./Contact/AddressbookModels.qml")
//    }
//    Loader {
//        id: imapLogDrawerLoader
//        width: parent.width
//        anchors.bottom: parent.bottom
////        height: parent.height
//        asynchronous: true
//        source: Qt.resolvedUrl("./DeveloperMode/ImapLogDrawer.qml")
//        active: GlobalSettings.developer.enableImapModelLogging && GlobalSettings.developer.developerModeEnabled
//    }

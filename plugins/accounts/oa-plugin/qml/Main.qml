/* Copyright (C) 2015 Dan Chapman <dpniel@ubuntu.com>

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
import Ubuntu.OnlineAccounts 0.1
import Ubuntu.Components.Popups 1.1
import accounts.plugin.Email 1.0

ListView {
    id: view

    property ServerSettings imap: ServerSettings{
        port: 143 // Set the default port for ServerConfig.CLEAR_TEXT
        networkMethod: ServerConfig.CLEAR_TEXT
    }
    property ServerSettings smtp: ServerSettings{
        port: 587 // Set the default port for ServerConfig.CLEAR_TEXT
        networkMethod: ServerConfig.CLEAR_TEXT
    }
    // A queue for dialogs/popups to ensure only one is on screen at a time.
    property PopUpQueue popupQueue: PopUpQueue{}

    // Used to signal to the calling OA process that setup is now done
    signal finished()

    function updateCredentials() {
        console.log("Debug: Updating credentials...")
        var dlgTitle = i18n.tr("Error")
        if (!imap.isValid()) {
            queueError(dlgTitle, i18n.tr("Invalid IMAP settings"))
            return
        }
        if (!smtp.isValid() && smtp._smtpEnabled) {
            queueError(dlgTitle, i18n.tr("Invalid SMTP settings"))
            return;
        }
        if (smtp._smtpEnabled) {
            imapCreds.synced.connect(_syncSmtpCreds)
            imap.updateCredentials(imapCreds)
        } else {
            imapCreds.synced.connect(credentialsStored)
            imap.updateCredentials(imapCreds)
        }
    }

    function _syncSmtpCreds() {
        smtpCreds.synced.connect(credentialsStored)
        smtp.updateCredentials(smtpCreds)
    }

    function credentialsStored() {
        // Disconnect these in case we need to authenticate again
        if (smtp._smtpEnabled) {
            imapCreds.synced.disconnect(_syncSmtpCreds)
            smtpCreds.synced.disconnect(credentialsStored)
        } else {
            imapCreds.synced.disconnect(credentialsStored)
        }

        console.log("Debug: Credentials stored, id: " + imapCreds.credentialsId)

        if (imapCreds.credentialsId === 0) {
            queueError(i18n.tr("Error"), i18n.tr("Failed storing account credentials"))
            return
        }
        globalAccountSettings.updateServiceEnabled(true)
        globalAccountSettings.credentials = imapCreds

        var settings = imap.updateGlobalSettings("IMAP", globalAccountSettings.settings, imapCreds.credentialsId)
        if (!smtp._smtpEnabled) {
            globalAccountSettings.updateSettings(settings)
        } else {
            globalAccountSettings.updateSettings(smtp.updateGlobalSettings("SMTP", settings, smtpCreds.credentialsId))
        }
        account.synced.connect(validateAccounts)
        account.sync()
    }

    function validateAccounts() {
        // Disconnect this here otherwise calling sync from
        // settings updates in the validation tasks causes an infinite loop :-)
        account.synced.disconnect(validateAccounts)
        // Queue the validation step: will execute tasks on component completion
        popupQueue.queue(Qt.resolvedUrl("ServiceValidationOverlay.qml"), view, {accountId: account.accountId, hasSmtp: smtp._smtpEnabled})
    }

    function handleVerifySsl(map) {
        sslCertView.certMap = map
        view.state = "verifySslCert"
    }

    function handleFailed(reason) {
        var protocol = reason["protocol"]
        var dlgTitle = protocol.toUpperCase() + i18n.tr(" Error")
        queueError(dlgTitle, reason["reason"])
    }

    function handleSuccess() {
        // TODO: is there anything else we need to do before finishing??
        finished()
    }

    function reloadAccountInfo() {
        imap.reloadGlobalSettings("IMAP", globalAccountSettings.settings, imapCreds)
        smtp.reloadGlobalSettings("SMTP", globalAccountSettings.settings, smtpCreds)
        serverDetails.reloadSettings()
    }

    function queueError (title, reason) {
        popupQueue.queue(Qt.resolvedUrl("ErrorDialog.qml"), view, {title: title, text: reason})
    }

    anchors {
        left: parent.left
        leftMargin: units.gu(2)
        top: parent.top
        topMargin: units.gu(1)
        right: parent.right
        rightMargin: units.gu(2)
        bottom: parent.bottom
    }

    anchors.fill: parent
    anchors.margins: units.gu(2)
    spacing: units.gu(1)

    footer: FooterButtons{
        onCancelClicked: view.finished()
        onContinueClicked: {
            if (view.state === "autoConfig") {
                autoDiscover.lookUp(imap.username)
            } else if (view.state === "verifySslCert") {
                var protocol = sslCertView.certMap["protocol"]
                if (sslCertView.certMap["self_signed"]) {
                    if (protocol === "imap") {
                        imap.allowSelfSigned = true;
                    } else if (protocol === "smtp") {
                        smtp.allowSelfSigned = true;
                    }
                } else if (sslCertView.certMap["untrusted"]) {
                    if (protocol === "imap") {
                        imap.allowUntrusted = true;
                    } else if (protocol === "smtp") {
                        smtp.allowUntrusted = true;
                    }
                }
                view.state = "serverInput"
                updateCredentials()
            } else {
                updateCredentials()
            }
        }
    }

    state: account.accountId === 0 ? "autoConfig" : "editDetails"
    states: [
        State {
            name: "autoConfig"
            PropertyChanges {
                target: view
                model: newAccount
            }
        },
        State {
            name: "serverInput"
            PropertyChanges {
                target: view
                model: serverDetails
            }
            PropertyChanges {
                target: serverDetails
                editMode: false
            }
            StateChangeScript {
                name: "reloadModel"
                script: serverDetails.reloadSettings()
            }
        },
        State {
            name: "editDetails"
            PropertyChanges {
                target: view
                model: serverDetails
            }
            PropertyChanges {
                target: serverDetails
                editMode: true
            }
            StateChangeScript {
                name: "reloadSettings"
                script: view.reloadAccountInfo()
            }
        },
        State {
            name: "verifySslCert"
            PropertyChanges {
                target: view
                model: sslCertView
            }
        }

    ]
    // Because we don't have a page stack due to the
    // the requirement that our root item is a flickable
    // we have to use a listview and visualitem models to navigate
    // the setup stages. Not ideal but it works :-)
    AutoConfigModel{id: newAccount}
    ServerDetailsModel{id: serverDetails}
    SslCertificateView{id: sslCertView}

    AutoDiscover {
        id: autoDiscover
        testMode: false
        function moveToServerDetails() {
            // For convenience assume the details
            // are the same for both servers, as more often
            // than not they are.
            smtp.username = imap.username
            smtp.password = imap.password
            view.state = "serverInput"
        }
        onInvalidMailAddress: console.log("Not a valid mailaddress: ", address)
        onNoNetworkAvailable: console.log("No network available, lookup aborted")
        onSuccess: {
            imap.server = serverConfig.imapHostName
            imap.port = serverConfig.imapPort
            imap.networkMethod = serverConfig.imapMethod
            imap.networkMethodString = serverConfig.toMap().imap.method // Useful for debugging
            smtp.server = serverConfig.smtpHostName
            smtp.port = serverConfig.smtpPort
            smtp.networkMethod = serverConfig.smtpMethod
            smtp.networkMethodString = serverConfig.toMap().smtp.method
            smtp._smtpEnabled = true
            moveToServerDetails()
        }
        onFailed: {
            // For the server etc we should probably add a guess here
            // e.g imap.domain.com instead of leaving it blank??
            moveToServerDetails()
        }
    }

    Credentials {
        id: imapCreds
        caption: account.provider.id
        acl: "*" // untill later
        storeSecret: true
    }

    Credentials {
        id: smtpCreds
        caption: account.provider.id
        acl: "*"
        storeSecret: true
        onCredentialsIdChanged: view.credentialsStored()
    }

    AccountService {
        id: globalAccountSettings
        objectHandle: account.accountServiceHandle
        autoSync: false
    }
}

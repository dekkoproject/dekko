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
import Ubuntu.Components 1.3
import Dekko.Accounts 1.0
import "../../actions/logging"
import "../../actions/views"
import "../../actions/settings"
import "../../stores/settings"
import "../components"

SettingsGroupPage {
    pageHeader.title: qsTr("Incoming Server")

    property AccountConfig incoming: account.incoming

    readonly property bool isIMAP: incoming ? incoming.serviceType === AccountConfig.IMAP : false
    readonly property bool isPOP: incoming ? incoming.serviceType === AccountConfig.POP3 : false

    function settingsChanged() {
        if (incoming.baseFolder !== basePath.text
            || incoming.name !== username.text
            || incoming.email !== username.text
            || incoming.password !== password.text
            || incoming.server !== server.text
            || incoming.port !== port.text
            || incoming.encryption !== encryption.selectedMethod
            || incoming.acceptUntrustedCertificates !== allowUntrustedSwitch.checked
            || (isIMAP && incoming.saslMechanism !== authentication.selectedMethod)
            || (isIMAP && incoming.idleEnabled !== idleEnabled.checked)
            || (isIMAP && incoming.isAutoDownload !== noMaxMailSize.checked)
            || (isIMAP && incoming.downloadAttachments !== autoDownloadAttachments.checked)
            || incoming.checkInterval !== parseInt(checkInterval.text)
            || incoming.checkWhenRoaming !== checkRoaming.checked
            || incoming.maxMailSize !== parseInt(maxMailSize.text)
            || incoming.canDeleteMail !== canDeleteMail.checked)
        {
            Log.logInfo("IncomingServerGroup::determineIfSettingsChanged", "Settings have changed")
            return true
        } else {
            Log.logInfo("IncomingServerGroup::determineIfSettingsChanged", "No changes")
            return false
        }
    }

    AppListener {
        Filter {
            type: SettingsKeys.saveCurrentGroup
            onDispatched: {
                Log.logInfo("IncomingServerGroup::saveCurrentGroup", "Saving group settings")
                if (!settingsChanged()) {
                    return
                }
                incoming.name = username.text
                incoming.email = username.text
                incoming.password = password.text
                incoming.server = server.text
                incoming.port = port.text
                incoming.encryption = encryption.selectedMethod
                incoming.acceptUntrustedCertificates = allowUntrustedSwitch.checked
                if (isIMAP) {
                    Log.logInfo("IncomingServerGroup::saveCurrentGroup", "Saving IMAP properties")
                    incoming.saslMechanism = authentication.selectedMethod
                    // incoming.checkOnStart = checkOnStart.checked
                    incoming.idleEnabled = idleEnabled.checked
                    incoming.checkInterval = parseInt(checkInterval.text)
                    incoming.checkWhenRoaming = checkRoaming.checked
                    incoming.maxMailSize = parseInt(maxMailSize.text)
                    incoming.isAutoDownload = noMaxMailSize.checked
                    incoming.downloadAttachments = autoDownloadAttachments.checked
                    incoming.canDeleteMail = canDeleteMail.checked
                } else if (isPOP) {
                    Log.logInfo("IncomingServerGroup::saveCurrentGroup", "Saving POP3 properties")
                    // incoming.checkOnStart = checkOnStart.checked
                    incoming.checkInterval = parseInt(checkInterval.text)
                    incoming.checkWhenRoaming = checkRoaming.checked
                    incoming.maxMailSize = parseInt(maxMailSize.text)
                    incoming.isAutoDownload = noMaxMailSize.checked
                    incoming.canDeleteMail = canDeleteMail.checked
                }
                SettingsStore.settingsChanged = true
                Log.logInfo("IncomingServerGroup::saveCurrentGroup", "Saved group settings")
            }
        }
    }

    PageFlickable {
        TitledTextField {
            id: server
            objectName: "serverInputField"
            title: qsTr("Hostname")
            placeholderText: "imap.example.org"
            inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhEmailCharactersOnly
            text: incoming ?  incoming.server : ""
        }

        TitledTextField {
            id: port
            objectName: "portInputField"
            title: qsTr("Port")
            placeholderText: "993"
            inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly
            text: incoming ?  incoming.port : ""
        }

        TitledTextField {
            id: username
            objectName: "usernameInputField"
            title: qsTr("Username")
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhEmailCharactersOnly
            placeholderText: "user@example.org"
            text: incoming ?  incoming.email : ""
        }

        TitledTextField {
            id: password
            objectName: "userInputPasswordField"
            title: qsTr("Password")
            inputMethodHints: Qt.ImhHiddenText | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            echoMode: showPassword.checked ? TextInput.Normal : TextInput.Password
            placeholderText: qsTr("Password")
            text: incoming ?  incoming.password : ""
        }

        CheckboxWithLabel {
            id: showPassword
            objectName: "showPwdCheckBox"
            text: qsTr("Show password")
            textColor: UbuntuColors.inkstone
        }

        SectionHeader {
            textMargin: 0
            text: qsTr("Security settings")
        }

        EncryptionSelector {
            id: encryption
            anchors {
                left: parent.left
                right: parent.right
            }
            Component.onCompleted: {
                if (incoming) {
                    setEncryptionType(incoming.encryption)
                }
            }
        }

        AuthenticationSelector {
            id: authentication
            anchors {
                left: parent.left
                right: parent.right
            }
            visible: incoming.serviceType !== AccountConfig.POP3

            Component.onCompleted: {
                if (visible && incoming) {
                    console.log("incomigauth", incoming.saslMechanism)
                    setAuthType(incoming.saslMechanism)
                }
            }
        }

        LabeledSwitch {
            id: allowUntrustedSwitch
            checked: incoming ? incoming.acceptUntrustedCertificates : false
            text: qsTr("Allow untrusted certificates")
        }

        SectionHeader {
            textMargin: 0
            text: qsTr("Server settings")
        }

        LabeledSwitch {
            id: checkOnStart
            text: qsTr("Check for new mail on start")
            checked: true
        }

        LabeledSwitch {
            id: idleEnabled
            text: qsTr("Enable IMAP IDLE")
            visible: incoming ? (isIMAP && incoming.canIDLE) : false
            checked: incoming ? incoming.idleEnabled : false
        }

        TitledTextField {
            id: checkInterval
            title: qsTr("Check interval (minutes)")
            text: incoming ? incoming.checkInterval : 5
            inputMethodHints: Qt.ImhDigitsOnly
        }

        LabeledSwitch {
            id: checkRoaming
            text: qsTr("Check when roaming")
            checked: incoming ? incoming.checkWhenRoaming : false
        }

        TitledTextField {
            id: maxMailSize
            title: qsTr("Maximum mail size (MB)")
            text: incoming ? incoming.maxMailSize : ""
        }

        CheckboxWithLabel {
            id: noMaxMailSize
            text: qsTr("No maximum mail size")
            textColor: UbuntuColors.inkstone
            checked: incoming ? incoming.isAutoDownload : false
        }

        LabeledSwitch {
            id: autoDownloadAttachments
            visible: isIMAP
            text: qsTr("Automatically download attachments")
            checked: incoming ? incoming.downloadAttachments : false
        }

        LabeledSwitch {
            id: canDeleteMail
            text: qsTr("Allowed to delete mail")
            checked: incoming ? incoming.canDeleteMail : false
        }
    }
}

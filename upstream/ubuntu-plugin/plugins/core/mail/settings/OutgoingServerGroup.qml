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
import Dekko.Mail.Accounts 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Settings 1.0
import Dekko.Ubuntu.Components 1.0
import "../components"

SettingsGroupPage {
    pageHeader.title: qsTr("Outgoing Server")

    property SmtpAccountConfig outgoing: account.outgoing

    function settingsChanged() {
        if (outgoing.username !== username.text
            || outgoing.password !== password.text
            || outgoing.server !== server.text
            || outgoing.port !== port.text
            || outgoing.encryption !== encryption.selectedMethod
            || outgoing.acceptUntrustedCertificates !== allowUntrustedSwitch.checked
            || outgoing.saslMechanism !== authentication.selectedMethod
            || outgoing.authFromCaps !== authFromCaps.checked)
        {
            Log.logInfo("OutgoingServerGroup::determineIfSettingsChanged", "Settings have changed")
            return true
        } else {
            Log.logInfo("OutgoingServerGroup::determineIfSettingsChanged", "No changes")
            return false
        }
    }

    AppListener {
        Filter {
            type: SettingsKeys.saveCurrentGroup
            onDispatched: {
                Log.logInfo("OutgoingServerGroup::saveCurrentGroup", "Saving group settings")
                if (!settingsChanged()) {
                    return
                }
                outgoing.username = username.text
                outgoing.password = password.text
                outgoing.server = server.text
                outgoing.port = port.text
                outgoing.encryption = encryption.selectedMethod
                outgoing.saslMechanism = authentication.selectedMethod
                outgoing.authFromCaps = authFromCaps.checked
                outgoing.acceptUntrustedCertificates = allowUntrustedSwitch.checked
                SettingsStore.settingsChanged = true
                Log.logInfo("OutgoingServerGroup::saveCurrentGroup", "Saved group settings")
            }
        }
    }

    PageFlickable {
        TitledTextField {
            id: server
            title: qsTr("Hostname")
            placeholderText: "smtp.example.org"
            inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhEmailCharactersOnly
            text: outgoing ?  outgoing.server : ""
        }

        TitledTextField {
            id: port
            title: qsTr("Port")
            placeholderText: "465"
            inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly
            text: outgoing ?  outgoing.port : ""
        }

        TitledTextField {
            id: username
            objectName: "usernameInputField"
            title: qsTr("Username")
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhEmailCharactersOnly
            placeholderText: "user@example.org"
            text: outgoing ?  outgoing.username : ""
        }

        TitledTextField {
            id: password
            title: qsTr("Password")
            inputMethodHints: Qt.ImhHiddenText | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            echoMode: showPassword.checked ? TextInput.Normal : TextInput.Password
            placeholderText: qsTr("Password")
            text: outgoing ?  outgoing.password : ""
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
                if (outgoing) {
                    setEncryptionType(outgoing.encryption)
                }
            }
        }

        AuthenticationSelector {
            id: authentication
            anchors {
                left: parent.left
                right: parent.right
            }

            Component.onCompleted: {
                if (visible && outgoing) {
                    setAuthType(outgoing.saslMechanism)
                }
            }
        }

        LabeledSwitch {
            id: authFromCaps
            checked: outgoing ? outgoing.authFromCaps : false
            text: qsTr("Authenticate from server capabilities")
        }

        LabeledSwitch {
            id: allowUntrustedSwitch
            checked: outgoing ? outgoing.acceptUntrustedCertificates : false
            text: qsTr("Allow untrusted certificates")
        }
    }
}

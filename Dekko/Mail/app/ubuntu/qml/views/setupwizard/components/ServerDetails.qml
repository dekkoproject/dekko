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
import "../../components"

Item {

    height: col.height
    anchors {
        left: parent.left
        right: parent.right
    }

    property string serviceType: "imap"
    property QtObject accountConfig: null

    // rather than using more property bindings here just return a map that can be
    // used for validation and saving to the account object.
    function toMap() {
        var s = {
            "servicetype" : serviceType,
            "hostname" : server.text,
            "port": port.text,
            "encryption": encryption.selectedMethod,
            "username": username.text,
            "password": password.text,
            "authentication": authentication.selectedMethod,
            "allowUntrusted": allowUntrustedSwitch.checked
        }
        return s
    }

    function showInvalidFields(result) {
        server.requiredField = result.indexOf("hostname") > -1
        port.requiredField = result.indexOf("port") > -1
        username.requiredField = result.indexOf("username") > -1
    }

    Column {
        id: col
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        spacing: units.gu(1)

        TitledTextField {
            id: server
            objectName: "serverInputField"
            title: qsTr("Hostname")
            placeholderText: "imap.example.org"
            inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhEmailCharactersOnly
            text: accountConfig ?  accountConfig.server : ""
        }

        TitledTextField {
            id: port
            objectName: "portInputField"
            title: qsTr("Port")
            placeholderText: "993"
            inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly
            text: accountConfig ? accountConfig.port : ""
        }

        EncryptionSelector {
            id: encryption
            anchors {
                left: parent.left
                right: parent.right
            }
            Component.onCompleted: {
                if (accountConfig) {
                    setEncryptionType(accountConfig.encryption)
                }
            }
        }

        TitledTextField {
            id: username
            objectName: "usernameInputField"
            title: qsTr("Username")
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhEmailCharactersOnly
            placeholderText: "user@example.org"
            text: accountConfig ? ((serviceType === "smtp") ? accountConfig.username : accountConfig.email) : ""
        }

        TitledTextField {
            id: password
            objectName: "userInputPasswordField"
            title: qsTr("Password")
            inputMethodHints: Qt.ImhHiddenText | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            echoMode: showPassword.checked ? TextInput.Normal : TextInput.Password
            placeholderText: qsTr("Password")
            text: accountConfig ?  accountConfig.password : ""
        }

        CheckboxWithLabel {
            id: showPassword
            objectName: "showPwdCheckBox"
            text: qsTr("Show password")
            textColor: UbuntuColors.inkstone
        }

        AuthenticationSelector {
            id: authentication
            anchors {
                left: parent.left
                right: parent.right
            }
            visible: serviceType !== "pop3"
            Component.onCompleted: {
                if (visible && accountConfig) {
                    setAuthType(accountConfig.saslMechanism)
                }
            }
        }

        LabeledSwitch {
            id: allowUntrustedSwitch
            checked: accountConfig ? accountConfig.acceptUntrustedCertificates : false
            text: qsTr("Allow untrusted certificates")
        }
    }
}

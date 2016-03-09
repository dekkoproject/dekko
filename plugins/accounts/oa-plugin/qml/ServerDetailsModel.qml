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
import Ubuntu.Components 1.3
import accounts.plugin.Email 1.0

VisualItemModel {
    property bool editMode: true

    function reloadSettings() {
        // just to avoid nasty binding loops
        imapServer.text = imap.server
        imapUser.text = imap.username
        imapPass.text = imap.password
        imapNetMethod.setCurrentIndex(imap.networkMethod)
        imapPort.text = parseInt(imap.port)
        smtpServer.text = smtp.server
        smtpPort.text = smtp.port
        smtpUser.text = smtp.username
        smtpPass.text = smtp.password
        smtpNetMethod.setCurrentIndex(smtp.networkMethod)
    }

    SectionDivider {
        text: i18n.tr("IMAP server")
        width: parent ? parent.width : 0
    }
    Label {
        text: i18n.tr("Username:")
        anchors.left: parent ? parent.left : undefined
        anchors.right: parent ? parent.right : undefined
    }

    TextField {
        id: imapUser
        objectName: "usernameField"
        placeholderText: i18n.tr("Your username")
        anchors.left: parent ? parent.left : undefined
        anchors.right: parent ? parent.right : undefined
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: imapPass
        onTextChanged: imap.username = text
    }

    Label {
        text: i18n.tr("Password:")
        anchors.left: parent ? parent.left : undefined
        anchors.right: parent ? parent.right : undefined
    }

    TextField {
        id: imapPass
        objectName: "passwordField"
        placeholderText: i18n.tr("Your password")
        echoMode: TextInput.Password
        anchors.left: parent ? parent.left : undefined
        anchors.right: parent ? parent.right : undefined
        inputMethodHints: Qt.ImhSensitiveData
        onTextChanged: imap.password = text
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: imapServer
    }

    Label {
        text: i18n.tr("Hostname:")
        anchors.left: parent ? parent.left : undefined
        anchors.right: parent ? parent.right : undefined
    }

    TextField {
        id: imapServer
        width: parent ? parent.width : 0
        onTextChanged: imap.server = text
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: imapPort
    }

    Label {
        text: i18n.tr("Port:")
        anchors.left: parent ? parent.left : undefined
        anchors.right: parent ? parent.right : undefined
    }

    TextField {
        id: imapPort
        width: parent ? parent.width : 0
        onTextChanged: imap.port = parseInt(text)
        KeyNavigation.priority: KeyNavigation.BeforeItem
        KeyNavigation.tab: smtpUser
    }

    Label {
        text: i18n.tr("Encryption method:")
        anchors.left: parent ? parent.left : undefined
        anchors.right: parent ? parent.right : undefined
    }

    EncryptionSelector {
        id: imapNetMethod
        width: parent ? parent.width : 0
        onSelectedIndexChanged: {
            var method = getMethod()
            if (imap.networkMethod !== method) {
                imap.networkMethod = method
            }
            switch(method) {
            case ServerConfig.CLEAR_TEXT:
            case ServerConfig.STARTTLS:
                if (parseInt(imapPort.text) === ServerConfig.PORT_IMAPS) {
                    imapPort.text = ServerConfig.PORT_IMAP;
                }
                break;
            case ServerConfig.SSL_TLS:
                if (parseInt(imapPort.text) === ServerConfig.PORT_IMAP) {
                    imapPort.text = ServerConfig.PORT_IMAPS;
                }
                break;
            }
        }
    }

    Label {
        text: i18n.tr("Authentication method:")
        anchors.left: parent ? parent.left : undefined
        anchors.right: parent ? parent.right : undefined
    }
    AuthSelector{
        onSelectedIndexChanged: {
            if (selectedIndex > 0) {
                imap.authMech = SaslMechanisms.toList()[selectedIndex]
            } else {
                imap.authMech = "AUTOMATIC"
            }
        }
        Component.onCompleted: {
            if (imap.authMech === "AUTOMATIC") {
                selectedIndex = 0
            } else {
                selectedIndex = SaslMechanisms.toList().indexOf(imap.authMech)
            }
        }
    }

    ListItemSwitch {
        text: i18n.tr("Allow self-signed certificates")
        width: parent ? parent.width : 0
        checked: imap.allowSelfSigned
        onCheckedChanged: {
            if (checked !== imap.allowSelfSigned) {
                imap.allowSelfSigned = checked
            }
        }
    }
    ListItemSwitch {
        text: i18n.tr("Allow untrusted certificates")
        width: parent ? parent.width : 0
        checked: imap.allowUntrusted
        onCheckedChanged: {
            if (checked !== imap.allowUntrusted) {
                imap.allowUntrusted = checked
            }
        }
    }

    SectionDivider {
        text: i18n.tr("SMTP server")
        width: parent ? parent.width : 0
    }
    ListItemSwitch {
        id: incSmtp
        text: i18n.tr("Include SMTP server")
        width: parent ? parent.width : 0
        checked: smtp._smtpEnabled
        onCheckedChanged: {
            if (checked !== smtp._smtpEnabled) {
                smtp._smtpEnabled = checked
            }
        }
    }

    Column {
        anchors.left: parent ? parent.left : undefined
        anchors.right: parent ? parent.right : undefined
        spacing: units.gu(1)

        Behavior on height {
            UbuntuNumberAnimation{}
        }

        Label {
            text: i18n.tr("Username:")
            visible: smtp._smtpEnabled
            anchors.left: parent ? parent.left : undefined
            anchors.right: parent ? parent.right : undefined
        }

        TextField {
            id: smtpUser
            visible: smtp._smtpEnabled
            placeholderText: i18n.tr("Your username")
            anchors.left: parent ? parent.left : undefined
            anchors.right: parent ? parent.right : undefined
            onTextChanged: smtp.username = text
            KeyNavigation.priority: KeyNavigation.BeforeItem
            KeyNavigation.tab: smtpPass
        }

        Label {
            text: i18n.tr("Password:")
            visible: smtp._smtpEnabled
            anchors.left: parent ? parent.left : undefined
            anchors.right: parent ? parent.right : undefined
        }

        TextField {
            id: smtpPass
            visible: smtp._smtpEnabled
            placeholderText: i18n.tr("Your password")
            echoMode: TextInput.Password
            anchors.left: parent ? parent.left : undefined
            anchors.right: parent ? parent.right : undefined
            inputMethodHints: Qt.ImhSensitiveData
            onTextChanged: smtp.password = text
            KeyNavigation.priority: KeyNavigation.BeforeItem
            KeyNavigation.tab: smtpServer
        }
        Label {
            visible: smtp._smtpEnabled
            text: i18n.tr("Hostname:")
            anchors.left: parent ? parent.left : undefined
            anchors.right: parent ? parent.right : undefined
        }

        TextField {
            id: smtpServer
            visible: smtp._smtpEnabled
            width: parent ? parent.width : 0
            onTextChanged: smtp.server = text
            KeyNavigation.priority: KeyNavigation.BeforeItem
            KeyNavigation.tab: smtpPort
        }

        Label {
            visible: smtp._smtpEnabled
            text: i18n.tr("Port:")
            anchors.left: parent ? parent.left : undefined
            anchors.right: parent ? parent.right : undefined
        }

        TextField {
            visible: smtp._smtpEnabled
            id: smtpPort
            width: parent ? parent.width : 0
            onTextChanged: smtp.port = parseInt(text)
        }
        Label {
            visible: smtp._smtpEnabled
            text: i18n.tr("Encryption method:")
            anchors.left: parent ? parent.left : undefined
            anchors.right: parent ? parent.right : undefined
        }

        EncryptionSelector {
            id: smtpNetMethod
            visible: smtp._smtpEnabled
            width: parent ? parent.width : 0
            onSelectedIndexChanged: {
                var method = model.get(selectedIndex).method
                if (smtp.networkMethod !== method) {
                    smtp.networkMethod = method
                }
                switch(method) {
                case ServerConfig.CLEAR_TEXT:
                case ServerConfig.STARTTLS:
                    if (parseInt(smtpPort.text) === ServerConfig.PORT_SSMTP) {
                        smtpPort.text = ServerConfig.PORT_SMTP;
                    }
                    break;
                case ServerConfig.SSL_TLS:
                    if (parseInt(smtpPort.text) === ServerConfig.PORT_SMTP) {
                        smtpPort.text = ServerConfig.PORT_SSMTP;
                    }
                    break;
                }
            }
        }

        Label {
            visible: smtp._smtpEnabled
            text: i18n.tr("Authentication method:")
            anchors.left: parent ? parent.left : undefined
            anchors.right: parent ? parent.right : undefined
        }
        AuthSelector{
            visible: smtp._smtpEnabled
            onSelectedIndexChanged: {
                if (selectedIndex > 0) {
                    smtp.authMech = SaslMechanisms.toList()[selectedIndex]
                } else {
                    smtp.authMech = "AUTOMATIC"
                }
            }
            Component.onCompleted: {
                if (smtp.authMech === "AUTOMATIC") {
                    selectedIndex = 0
                } else {
                    selectedIndex = SaslMechanisms.toList().indexOf(smtp.authMech)
                }
            }
        }
        ListItemSwitch {
            visible: smtp._smtpEnabled
            text: i18n.tr("Allow self-signed certificates")
            width: parent ? parent.width : 0
            checked: smtp.allowSelfSigned
            onCheckedChanged: {
                if (checked !== smtp.allowSelfSigned) {
                    smtp.allowSelfSigned = checked
                }
            }
        }
        ListItemSwitch {
            visible: smtp._smtpEnabled
            text: i18n.tr("Allow untrusted certificates")
            width: parent ? parent.width : 0
            checked: smtp.allowUntrusted
            onCheckedChanged: {
                if (checked !== smtp.allowUntrusted) {
                    smtp.allowUntrusted = checked
                }
            }
        }
    }

}

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
import accounts.plugin.Email 1.0

QtObject {
    id: settings

    property string username: ""
    property string password: ""
    property string server: ""
    property string authMech: "AUTOMATIC"
    property int port: 0
    property int networkMethod: ServerConfig.INVALID
    property string networkMethodString: ""
    property bool allowSelfSigned: false
    property bool allowUntrusted: false
    property bool shouldAuthenticate: true
    property bool _smtpEnabled: false

    function isValid() {
        // No need to check password as it may not be required by the user
        if (!username.length || !server.length) {
            return false
        }
        if (port === 0 || networkMethod === ServerConfig.INVALID) {
            return false
        }
        return true
    }

    function updateCredentials(creds) {
        creds.userName = username
        creds.secret = password
        creds.sync()
    }

    function updateGlobalSettings(protocol, globalSettings, credentialsID) {
        switch(protocol) {
        case "IMAP":
            globalSettings["name"] = username;
            globalSettings["imap/server/hostname"] = server
            globalSettings["imap/server/port"] = port
            globalSettings["imap/encryption/useStartTLS"] = networkMethod === ServerConfig.STARTTLS
            globalSettings["imap/encryption/useSSL"] = networkMethod === ServerConfig.SSL_TLS
            globalSettings["imap/authentication/sasl/mechanism"] = authMech
            globalSettings["imap/certificates/allowUntrusted"] = allowUntrusted
            globalSettings["imap/certificates/allowSelfSigned"] = allowSelfSigned
            globalSettings["imap/certificates/currentPubKey"] = ""
            globalSettings["imap/authentication/shouldAuthenticate"] = shouldAuthenticate
            break
        case "SMTP":
            globalSettings["smtp/server/hostname"] = server
            globalSettings["smtp/server/port"] = port
            globalSettings["smtp/encryption/useStartTLS"] = networkMethod === ServerConfig.STARTTLS
            globalSettings["smtp/encryption/useSSL"] = networkMethod === ServerConfig.SSL_TLS
            globalSettings["smtp/CredentialsId"] = credentialsID
            globalSettings["smtp/authentication/sasl/mechanism"] = authMech
            globalSettings["smtp/certificates/allowUntrusted"] = allowUntrusted
            globalSettings["smtp/certificates/allowSelfSigned"] = allowSelfSigned
            globalSettings["smtp/certificates/currentPubKey"] = ""
            globalSettings["smtp/authentication/shouldAuthenticate"] = shouldAuthenticate
            break
        }
        return globalSettings
    }

    function reloadGlobalSettings(protocol, globalSettings, credentials) {
        switch(protocol) {
        case "IMAP":
            server = globalSettings["imap/server/hostname"]
            port = globalSettings["imap/server/port"]
            if (globalSettings["imap/encryption/useSSL"]) {
                networkMethod = ServerConfig.SSL_TLS
            } else if (globalSettings["imap/encryption/useStartTLS"] ) {
                networkMethod = ServerConfig.STARTTLS
            } else {
                networkMethod = ServerConfig.CLEAR_TEXT
            }
            shouldAuthenticate = globalSettings["imap/authentication/shouldAuthenticate"]
            break
        case "SMTP":
            server = globalSettings.smtp.server.hostname
            port = globalSettings.smtp.server.port
            if (globalSettings.smtp.encryption.useSSL) {
                networkMethod = ServerConfig.SSL_TLS
            } else if (globalSettings.smtp.encryption.useStartTLS) {
                networkMethod = ServerConfig.STARTTLS
            } else {
                networkMethod = ServerConfig.CLEAR_TEXT
            }
            shouldAuthenticate = globalSettings["smtp/authentication/shouldAuthenticate"]
            break
        }
        username = credentials.userName
        password = credentials.secret
    }

    function clear() {
        username = ""
        password = ""
        port = 0
        server = ""
        networkMethod = ServerConfig.INVALID
    }
}

/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

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
import QtTest 1.0
import MailAutoDiscover 1.0
import "../../qml"

TestCase {
    name: "ServerSettingsTests"

    property var settings
    property var credentials

    ServerSettings {
        id: imapSettings
    }

    function test_reload_settings() {
        imapSettings.reloadGlobalSettings("IMAP", settings, credentials)
        assertReloadedSettings(imapSettings)
    }

    function test_update_settings() {
        var newHostname = "my.other.server.com"
        var newPort = 5000
        imapSettings.reloadGlobalSettings("IMAP", settings, credentials)
        assertReloadedSettings(imapSettings)
        imapSettings.server = newHostname
        imapSettings.port = newPort
        imapSettings.shouldAuthenticate = true
        imapSettings.updateGlobalSettings("IMAP", settings, credentials)
        compare(settings["imap/server/hostname"], newHostname, "Hostname didn't change")
        compare(settings["imap/server/port"], newPort, "Port didn't change")
        compare(settings["imap/authentication/shouldAuthenticate"], true, "ShouldAuthenticate not true")
    }

    function test_settings_isValid() {
        compare(imapSettings.isValid(), false, "Settings not INVALID")
        imapSettings.reloadGlobalSettings("IMAP", settings, credentials)
        assertReloadedSettings(imapSettings)
        compare(imapSettings.isValid(), true, "Settings were INVALID")
    }

    function assertReloadedSettings(settings) {
        compare(imapSettings.server, "imap.example.com", "Hostnames don't match")
        compare(imapSettings.networkMethod, ServerConfig.SSL_TLS, "Network encryption methods don't match")
        compare(imapSettings.port, 993, "Host ports don't match")
        compare(imapSettings.username, "dpniel@ubuntu.com", "Usernames don't match")
        compare(imapSettings.password, "my_password", "Password don't match")
        compare(imapSettings.shouldAuthenticate, false, "ShouldAuthenticate not false")
    }


    function init() {
        settings = {
            "imap/server/hostname": "imap.example.com",
            "imap/server/port": 993,
            "imap/encryption/useSSL": true,
            "imap/encryption/useStartTLS": true,
            "imap/authentication/shouldAuthenticate": false,
            "imap/authentication/sasl/mechanism": "AUTOMATIC",
            "imap/certificates/acceptSelfSigned": true,
            "imap/certificates/acceptUntrusted": false
        }

        credentials = {
            userName: "dpniel@ubuntu.com",
            secret: "my_password"
        }
    }

    function cleanup() {
        imapSettings.clear()
    }
}

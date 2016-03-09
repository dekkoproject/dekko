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
import accounts.plugin.Email 1.0

TestCase {
    name: "AutoDiscoverTests"

    AutoDiscover {
        id: autoDiscover
        testMode: true
    }

    SignalSpy {
        id: spy
        target: autoDiscover
    }

    function test_invalid_mailaddress() {
        spy.signalName = "invalidMailAddress"
        compare(spy.count, 0)
        autoDiscover.lookUp("invalid_address")
        compare(spy.count, 1)
    }

    function test_autodiscover_success() {
        spy.signalName = "success"
        compare(spy.count, 0)
        autoDiscover.lookUp("dpniel@ubuntu.com")
        spy.wait(500)
        compare(spy.count, 1)
        validateServerConfig(autoDiscover._serverConf)
    }

    function validateServerConfig(config) {
        compare(config.isValid, true)
        compare(config.imapHostName, "imap.example.com")
        compare(config.imapPort, ServerConfig.PORT_IMAPS)
        compare(config.imapMethod, ServerConfig.SSL_TLS)
        compare(config.smtpHostName, "smtp.example.com")
        compare(config.smtpPort, ServerConfig.PORT_SMTP)
        compare(config.smtpMethod, ServerConfig.STARTTLS)
    }

    function cleanup() {
        spy.clear()
    }

}

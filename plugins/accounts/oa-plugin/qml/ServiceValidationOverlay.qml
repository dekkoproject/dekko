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
import Ubuntu.Components.Popups 1.1
import accounts.plugin.Email 1.0

Dialog {
    id: overlay

    property alias accountId: validator.accountId
    property alias validator: validator
    readonly property bool isRunning: state === "running"
    property bool hasSmtp: false

    signal verifySsl(var map)
    signal success()
    signal failed(var reason)
    signal closing()
    Component.onDestruction: closing()

    function close() {
        PopupUtils.close(overlay)
    }

    state: "running"
    states: [
        State {
            name: "running"
            PropertyChanges {
                target: overlay
                title: i18n.tr("Validating account.")
            }
        },
        State {
            name: "complete"
            PropertyChanges {
                target: overlay
                title: i18n.tr("Account validated.")
            }
        },
        State {
            name: "failed"
            PropertyChanges {
                target: overlay
                title: i18n.tr("Account validation failed.")
            }
        }
    ]

    contents: Item {
        width: parent.width
        height: units.gu(6)
        ActivityIndicator {
            id: activity
            running: true
            anchors.centerIn: parent
        }
    }

    Timer {
        id: successTimer
        interval: 500
        repeat: false
        onTriggered: {
            view.handleSuccess()
            overlay.close()
        }
    }

    Timer {
        id: start
        interval: 1000
        repeat: false
        onTriggered: validator.validate(account.objectHandle, hasSmtp)
    }

    Component.onCompleted: start.start()

    ServiceValidator {
        id: validator
        onValidated: {
            overlay.state = "complete"
            successTimer.start()
        }

        onVerifyCertificate: {
            view.handleVerifySsl(map)
            overlay.close()
        }

        onFailed: {
            overlay.state = "failed"
            view.handleFailed(map)
            overlay.close()
        }

        onStatusChanged: {
            switch(status) {
            case ServiceValidator.INVALID:
                printState("INVALID")
                break
            case ServiceValidator.RUNNING:
                printState("RUNNING")
                break
            case ServiceValidator.FINISHED:
                printState("FINISHED")
                break
            }
        }
        function printState(state) {
            console.log("[ServiceValidator] Current state: ", state)
        }
    }
}


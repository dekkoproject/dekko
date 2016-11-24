/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of the Dekko Project

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing,
   software distributed under the License is distributed on an
   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
   either express or implied. See the License for the specific
   language governing permissions and limitations under the License.
*/
import QtQuick 2.4

QtObject {
    readonly property bool notificationOnScreen: d.notificationVisible
    readonly property int notificationCount: d.notifications.length

    function notify(notification, caller, properties) {
        d.notifications.push(notification)
        d.callers.push(caller)
        d.properties.push(properties)
        timer.start()
    }

    property QtObject __internal: QtObject {
        id: d
        property bool notificationVisible: false
        property var notifications: []
        property var callers: []
        property var properties: []
        property var currentNotification

        property Timer timer: Timer {
            id: timer
            interval: 50
            onTriggered: {
                console.log("[dekko-notify] >> Checking notification queue")
                if (!d.notificationVisible && d.notifications.length) {
                    console.log("[dekko-notify] >> Notification pending. Showing now!")
                    d.show(d.notifications.shift(), d.callers.shift(), d.properties.shift())
                } else if (d.notificationVisible) {
                    console.log("[dekko-notify] >> Notification already on screen, waiting for completion")
                } else {
                    console.log("[dekko-notify] >> No notifications to show")
                }
            }
        }
        function show(notification, caller, properties) {
            var c = Qt.createComponent(notification)

            if (c.status === Component.Error) {
                console.log("Error: ", c.errorString())
                return
            }

            d.currentNotification = c.createObject(caller, properties)
            d.currentNotification.closed.connect(d.handleClosed)
            d.notificationVisible = true
        }
        function handleClosed() {
            console.log("[dekko-notify] >> Notification complete. Destroying....")
            d.notificationVisible = false
            d.currentNotification.destroy()
            console.log("[dekko-notify] >> Notification destroyed.")
            timer.start()
        }
    }
}

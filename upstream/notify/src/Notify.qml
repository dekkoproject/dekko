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
import Ubuntu.Components 1.3

Item {
    id: notify
    anchors.fill: parent
    property alias notificationOnScreen: queue.notificationOnScreen
    property alias count: queue.notificationCount

    property NotifyConfiguration config: NotifyConfiguration{}

    function send(title, body, props) {
        if (typeof body === "object" || body instanceof Object) {
            console.log("[dekko-notify] >> Properties passed as body argument. Correcting....")
            props = body
            body = ""
        }
        if (props) {
            console.log("[dekko-notify] >> Queueing custom notification")
            sendCustom(title, body, props)
        } else if (body) {
            if (typeof body === "string" || body instanceof String) {
                console.log("[dekko-notify] >> Queueing subtitles notification")
                sendStandard(title, body)
            } else {
                console.error("[dekko-notify] >> Invalid body type.")
            }
        } else {
            console.log("[dekko-notify] >> Queueing simple notification")
            sendStandard(title, "")
        }
    }

    function sendStandard(title, body) {
        sendCustom(title, body, notify.config)
    }

    function sendCustom(title, body, conf) {
        var props = {
            "title" : title,
            "body" : body,
            "action": conf.action,
            "position" : conf.position,
            "displayTime": conf.displayTime,
            "iconName" : conf.iconName,
            "iconColor" : conf.iconColor
        }
        queue.notify(conf.source, QuickUtils.rootItem(notify), props)
    }

    NotifyQueue {
        id: queue
    }
}

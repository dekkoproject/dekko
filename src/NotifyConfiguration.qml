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

    property url source: Qt.resolvedUrl("./Notification.qml")

    // Options are Item.Left, Item.Center, Item.Right
    property int position: Item.Right

    property int displayTime: 3000 // 3 seconds

    property Action action

    property color iconColor: UbuntuColors.slate

    property string iconName: ""
}

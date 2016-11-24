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

NotificationBase {
    id: base

    anchors.fill: parent

    UbuntuShape {
        id: notification
        aspect: UbuntuShape.DropShadow
        backgroundColor: UbuntuColors.porcelain
        width: units.gu(30)
        height: layout.height

        anchors.margins: units.gu(3)

        ListItemLayout {
            id: layout
            title.text: base.title
            subtitle.text: base.body
            subtitle.maximumLineCount: 4

            Icon {
                height: units.gu(4)
                width: height
                visible: base.hasIcon
                name: base.hasIcon && base.iconName ? base.iconName : ""
                color: base.iconColor
                SlotsLayout.position: SlotsLayout.Leading
            }
        }

        AbstractButton {
            anchors.fill: parent
            action: base.action
            visible: base.action
            onClicked: {
                if (destroyAnimation.running) {
                    destroyAnimation.pause()
                }
                base.closed()
            }
        }

        SequentialAnimation {
            id: showAnimation
            running: true
            NumberAnimation { target: notification; property: "opacity"; to: 1.0; duration: 500 }
            PauseAnimation { duration: base.displayTime }
            ScriptAction { script: destroyAnimation.restart() }
        }

        SequentialAnimation {
            id: destroyAnimation
            NumberAnimation { target: notification; property: "opacity"; to: 0; duration: 500 }
            ScriptAction { script: base.closed() }
        }
    }

    states: [
        State {
            when: base.dockedLeft
            AnchorChanges {
                target: notification;
                anchors {
                    top: base.top;
                    left: base.left;
                    right: undefined;
                    bottom: undefined;
                }
            }
        },
        State {
            when: base.dockedRight
            AnchorChanges {
                target: notification;
                anchors {
                    top: base.top;
                    left: undefined;
                    right: base.right;
                    bottom: undefined;
                }
            }
        },
        State {
            when: base.dockedCenter
            AnchorChanges {
                target: notification;
                anchors {
                    top: base.top;
                    horizontalCenter: base.horizontalCenter;
                }
            }
        }
    ]
}

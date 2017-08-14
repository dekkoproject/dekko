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
import Ubuntu.Components.Popups 1.3
import Dekko.Components 1.0
import QuickFlux 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Composer 1.0
import Dekko.Ubuntu.Components 1.0
import Dekko.Ubuntu.Constants 1.0

PopupBase {
    id: base

    readonly property int maxHeight: units.gu(70)
    readonly property int maxWidth: units.gu(100)

    Rectangle {
        anchors.fill: parent
        color: UbuntuColors.graphite
        opacity: 0.6
    }

    UbuntuShape {
        readonly property bool shouldCenter: height === base.maxHeight
        readonly property bool addTopMargin: height > units.gu(40)
        aspect: UbuntuShape.DropShadow
        width: Math.min((parent.width / 3)*2, base.maxWidth)
        height: Math.min((parent.height / 5)*4, base.maxHeight)
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: shouldCenter ? undefined : parent.top
            topMargin: shouldCenter ? undefined : (addTopMargin ? units.gu(6) : Style.defaultSpacing)
            verticalCenter: shouldCenter ? parent.verticalCenter : undefined
        }
        color: UbuntuColors.porcelain
        clip: true

        StretchColumn {
            anchors.fill: parent
            Item {
                anchors {
                    top: parent.top
                    right: parent.right
                    left: parent.left
                }
                height: units.gu(6)
                implicitHeight: height

                StretchRow {
                    anchors.fill: parent
                    anchors.leftMargin: units.gu(1)
                    anchors.rightMargin: units.gu(1)
                    spacing: units.gu(1)
                    TitledHeaderButton {
                        height: units.gu(5)
                        anchors.verticalCenter: parent.verticalCenter
                        action: ComposerStore.actions.discardMessageAction
                    }
                    TitledHeaderButton {
                        height: units.gu(5)
                        anchors.verticalCenter: parent.verticalCenter
                        action: ComposerStore.actions.saveDraftAction
                    }

                    TitledHeaderButton {
                        height: units.gu(5)
                        anchors.verticalCenter: parent.verticalCenter
                        action: ComposerStore.actions.attachmentsAction
                    }

                    Stretcher {}

                    TitledHeaderButton {
                        height: units.gu(5)
                        anchors.verticalCenter: parent.verticalCenter
                        action: ComposerStore.actions.sendAction
                    }
                }

                Line {
                    anchors {
                        bottom: parent.bottom
                        right: parent.right
                        left: parent.left
                    }
                }
            }

            Stretcher {
                id: content
                Rectangle {
                    anchors.fill: parent
                    color: "#FFFFFF"
                }
                StretchRow {
                    anchors.fill: parent
                    spacing: 0

                    Stretcher {
                        implicitHeight: parent.height
                        MessageComposer {
                            anchors.fill: parent
                        }
                    }
                    ComposeSidePanel {}
                }
            }

            Item {
                id: statusBar
                height: units.gu(3)
                width: parent.width
                implicitWidth: width
                implicitHeight: height
                Label {
                    anchors {
                        left: parent.left
                        leftMargin: Style.defaultSpacing
                        verticalCenter: parent.verticalCenter
                    }
                    fontSize: "small"
                    text: "TODO: background status' like \"Saving draft....\""
                }
            }
        }
    }
    AppListener {
        enabled: dekko.isLargeFF
        filter: ViewKeys.closeComposer
        onDispatched: {
            PopupUtils.close(base)
        }
    }
}


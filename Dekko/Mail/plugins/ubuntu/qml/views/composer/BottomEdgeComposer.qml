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
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Composer 1.0
import QuickFlux 1.0
import "../components"
import "../../constants"

BottomEdgeConfiguration {
    id: bc
    sourceComponent: DekkoPage {
        width: dekkoPage.width
        height: dekkoPage.height
        Rectangle {
            anchors.fill: parent
            color: "#FFFFFF"
        }
        pageHeader.title: "New message"
        pageHeader.backAction: Action {
            iconName: "down"
            onTriggered: ComposerActions.discardMessage()
        }
        pageHeader.primaryAction: ComposerStore.actions.attachmentsAction
        pageHeader.secondaryActions: ComposerStore.actions.sendAction

        MessageComposer {
            id: composer
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
                bottom: ap.visible ? ap.top : parent.bottom
            }
        }

        ExpandablePanel {
            id: ap
            anchors {
                left: parent.left
                bottom: parent.bottom
                right: parent.right
            }
            maxHeight: composer.height - Style.defaultSpacing
            visible: ComposerStore.attachments.count > 0 && !Qt.inputMethod.visible
            countText: ComposerStore.attachments.count
            text: qsTr("Attachments")
            iconName: Icons.AttachmentIcon
            Repeater {
                model: ComposerStore.attachments
                delegate: ListItemWithActions {
                    id: ad
                    property Component openItem: Item{}
                    property var attachment: model.qtObject
                    height: aLayout.implicitHeight
                    width: parent.width
                    showDivider: true
                    triggerIndex: model.index
                    leftSideAction: ComposerStore.actions.deleteAttachment
        //            rightSideActions: [flagAction, readAction, contextAction]
                    onItemClicked: {
                        if (mouse.button === Qt.RightButton) {
                            PopupUtils.open(Qt.resolvedUrl("../popovers/AttachmentPopover.qml"),
                                                                 ad,
                                                                 {
                                                                     index: model.index,
                                                                     attachment: model.qtObject
                                                                 })
                            return;
                        }
                        Log.logInfo("AttachmentList::openAttachment", "Attachment octet size is: %1".arg(model.qtObject.sizeInBytes))
                        // TODO; If attachment.partType === Attachment.File {
                        Qt.openUrlExternally("file:///%1".arg(attachment.url))
                        // } else {
                        // model.qtObject.open(openItem.createObject())
                        // }
                    }
                    ListItemLayout {
                        id: aLayout
                        title.text: attachment.displayName
                        subtitle.text: attachment.size

                        Icon {
                            source: Paths.mimeIconForUrl("file:///%1".arg(attachment.url))
                            color: "#888888"
                            height: Style.largeSpacing; width: height
                            SlotsLayout.position: SlotsLayout.Leading
                        }

                        Connections {
                            target: model.qtObject
                            onReadyToOpen: {
                                Qt.openUrlExternally(url)
                            }
                        }
                    }
                }
            }
        }

        AppScript {
            runWhen: ViewKeys.closeComposer
            enabled: listenerEnabled
            script: {
                if (!dekko.isLargeFF) {
                    bottomEdge.collapse()
                } else {
                    exit.bind(this, 0)
                }
            }
        }
        AppListener {
            enabled: !dekko.isLargeFF
            filter: ViewKeys.closeComposer
            onDispatched: {
                bottomEdge.collapse()
            }
        }
    }
}


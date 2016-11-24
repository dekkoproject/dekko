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
import "../../actions/composer"
import "../../actions/logging"
import "../../constants"
import "../../stores/composer"
import "../components"
import "../delegates"

ScrollView {
    anchors.fill: parent
    ListView {
        anchors.fill: parent
        add: DekkoAnimation.listViewAddTransition
        addDisplaced: DekkoAnimation.listViewAddDisplacedTransition
        remove: DekkoAnimation.listViewRemoveTransition
        removeDisplaced: DekkoAnimation.listViewRemoveDisplacedTransition
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
                    PopupUtils.open(Qt.resolvedUrl("qrc:/qml/views/popovers/AttachmentPopover.qml"),
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

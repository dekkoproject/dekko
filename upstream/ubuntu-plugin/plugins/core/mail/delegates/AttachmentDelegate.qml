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
import Dekko.Components 1.0
import Dekko.Mail.API 1.0
import Dekko.Ubuntu.Constants 1.0
import Dekko.Ubuntu.Components 1.0

ListItem {
    property Component openItem: Item{}
    property var attachment: model.qtObject
    height: aLayout.height

    Connections {
        target: model.qtObject
        onReadyToOpen: {
            if (isRunningOnMir) {
                ContentActions.exportFile(url)
            } else {
                Qt.openUrlExternally(url)
            }
        }
    }
    onClicked: {
        Log.logInfo("AttachmentPanel::openAttachment", "Attachment octet size is: %1".arg(model.qtObject.sizeInBytes))
        // This really is a crude hack
        // the attachments object can't directly
        // access the custom qnam factory. So we pass
        // it a QObject from the qml context which
        // the attachment object can access the QQmlEngine from
        // FIXME: Refactor Client* classes into seperate lib
        model.qtObject.open(openItem.createObject())
    }
    ListItemLayout {
        id: aLayout
        height: units.gu(6)
        title.text: attachment.displayName
        subtitle.text: attachment.mimeType + ", " + attachment.size
        Icon {
            source: Paths.mimeIconForMimeType(attachment.mimeType)
            color: "#888888"
            height: Style.largeSpacing; width: height
            SlotsLayout.position: SlotsLayout.Leading
        }
        ActivityIndicator {
            visible: running
            running: attachment.fetchInProgress
        }

        CachedImage {
            height: Style.defaultSpacing
            width: height
            name: Icons.ContextMenuIcon
            AbstractButton {
                anchors.fill: parent
                onClicked: PopupActions.showNotice("Not implemented. Fix before release")
            }
        }
    }
}

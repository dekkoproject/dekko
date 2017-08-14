/* Copyright (C) 2014-2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu Devices/

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
import Ubuntu.Components.Popups 1.0 as Popups
import Ubuntu.Content 1.3
import Dekko.Ubuntu.Constants 1.0

Popups.PopupBase  {
    id: picker
    z: 100

    property bool isExport: true

    // The url of the file to export, used to determine content type filter of peer picker
    property string fileUrl

    property var activeTransfer
    property var selectedItems
    property string downloadFilePath

    signal complete();
    signal filesImported(var files);

    Rectangle {
        anchors.fill: parent

        ContentTransferHint {
            anchors.fill: parent
            activeTransfer: picker.activeTransfer
        }

        ContentPeerPicker {
            id: peerPicker
            anchors.fill: parent
            visible: true
            contentType:  isExport ? Content.resolveType(fileUrl) : ContentType.All
            handler: isExport ? ContentHandler.Destination : ContentHandler.Source
            onPeerSelected: {
                peer.selectionType = (isExport ? ContentTransfer.Single : ContentTransfer.Multiple)
                picker.activeTransfer = peer.request()
                stateChangeConnection.target = picker.activeTransfer
            }
            onCancelPressed: {
                PopupUtils.close(picker)
                picker.complete();
            }
        }
    }

    Connections {
        id: stateChangeConnection
        onStateChanged: {
            if (isExport && picker.activeTransfer.state === ContentTransfer.InProgress) {
                picker.activeTransfer.items = [transferComponent.createObject(dekko, {"url": fileUrl}) ]
                picker.activeTransfer.state = ContentTransfer.Charged;
                closeTimer.start();
            } else if (!isExport && picker.activeTransfer.state === ContentTransfer.Charged){
                picker.filesImported(picker.activeTransfer.items);
                closeTimer.start();
            }
        }
    }
    Timer {
        id: closeTimer
        interval: 1000
        repeat: false
        onTriggered: {
            PopupUtils.close(picker);
            picker.complete();
        }
    }
    Component {
        id: transferComponent
        ContentItem {}
    }
}

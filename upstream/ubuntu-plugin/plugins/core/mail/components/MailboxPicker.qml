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
import Dekko.Mail 1.0
import "../delegates"
import Dekko.Ubuntu.Constants 1.0

Item {
    id: picker

    property alias accountId: folderList.accountId
    property bool contextActionsEnabled: false
    signal folderSelected(string folderName, var folderKey)
    signal folderClicked(int folderId)
    signal folderTypeClicked(int folderType, int folderId)
    signal folderPicked(var folder)

    width: parent.width
    height: parent.height

    FolderList {
        id: folderList
    }

    ScrollView {
        anchors.fill:parent
        ListView {
            id: lv
            anchors.fill: parent
            add: DekkoAnimation.listViewAddTransition
            addDisplaced: DekkoAnimation.listViewAddDisplacedTransition
            remove: DekkoAnimation.listViewRemoveTransition
            removeDisplaced: DekkoAnimation.listViewRemoveDisplacedTransition
            highlight: !dekko.isSmallFF ? Style.highlightBar : null
            highlightFollowsCurrentItem: true
            highlightMoveDuration: 200
            currentIndex: -1
            model: folderList.model
            delegate: FolderListDelegate {
                actionsEnabled: picker.contextActionsEnabled
                folder: model.qtObject
                onItemClicked: {
                    lv.currentIndex = model.index
                    picker.folderSelected(folder.name, folder.messageKey)
                    picker.folderClicked(folder.folderId)
                    picker.folderTypeClicked(folder.type, folder.folderId)
                    picker.folderPicked(folder)
                }
            }
        }
    }
}

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
import Dekko.Components 1.0
import "./components"
import "../actions/views"
import "../actions/messaging"
import "../constants"

DekkoPage {
    id: contactFilterView

    property string pickerId
    // the messages parentAccountId
    property alias accountId: pick.accountId

    pageHeader.title: qsTr("Select folder")
    pageHeader.backAction: Action {
        id: back
        iconName: "back"
        onTriggered: {
            MailboxActions.moveMessageCancelled(pickerId)
            ViewActions.popStageArea(ViewKeys.messageListStack)
        }
    }
    extendHeader: !dekko.viewState.isSmallFF

    PageContent {
        MailboxPicker {
            id: pick
            anchors.fill: parent
            onFolderTypeClicked: {
                MailboxActions.folderSelected(pickerId, folderType, folderId)
                ViewActions.popStageArea(ViewKeys.messageListStack)
            }
        }
    }
}

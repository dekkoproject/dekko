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
import "./delegates"
import "../actions/views"
import "../actions/messaging"

DekkoPage {

    property string pageTitle: "Folders"
    property alias accountId: folderList.accountId

    pageHeader.title: pageTitle
    pageHeader.backAction: Action {
        iconName: "back"
        onTriggered: ViewActions.popStageArea(dekko.viewState.isLargeFF ? ViewKeys.navigationStack : ViewKeys.messageListStack)
    }
    extendHeader: !dekko.viewState.isSmallFF

    FolderList {
        id: folderList
    }

    PageContent {

        ScrollView {
            anchors {
                fill: parent
            }
            ListView {
                anchors.fill: parent
                clip: true
                model: folderList.model
                delegate: FolderListDelegate {
                    folder: model.qtObject
                    onItemClicked: MessageActions.openFolder(folder.name, folder.messageKey)
                }
            }
        }
    }
}


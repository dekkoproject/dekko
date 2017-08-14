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
import QuickFlux 1.0
import Dekko.Ubuntu.Constants 1.0

StyledItem {
    id: dekkoPage

    property alias pageHeader: h
    property alias selectedIndex: h.selectedIndex
    property alias isSearchMode: h.isSearchMode
    property alias extendHeader: h.extend

    default property alias content: mainContent.data

    signal searchActivated(string searchString)
    signal searchCanceled()
    signal multiSelectCanceled()

    function startMultiSelect() {
        h.startMultiSelect()
    }
    clip: true

    DekkoHeader {
        id: h
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        onSearchActivated: dekkoPage.searchActivated(searchString)
        onSearchCanceled: dekkoPage.searchCanceled()
        onMultiSelectCanceled: dekkoPage.multiSelectCanceled()
        onComposeNewMessage: ViewActions.openComposer()
    }

    Item {
        id: mainContent
        anchors {
            left: parent.left
            top: h.bottom
            right: parent.right
            bottom: parent.bottom
//            bottomMargin: bel.active && bel.item.hint.visible ? bel.item.hint.height : 0
        }
        // MAIN CONTENT HERE
    }
}

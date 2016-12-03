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
import Dekko.Mail.API 1.0
import Dekko.Mail.Accounts 1.0
import Dekko.Mail.Settings 1.0
import "./components" as Comps
import "./models"

Comps.DekkoPage {
    id: menuPage

    pageHeader.title: qsTr("Mail")
    pageHeader.composeButtonEnabled: tabBar.currentIndex === 0 && dekko.isLargeFF
    extendHeader: true

    Comps.PageContent {

        ListView {
            id: navDrawer
            clip: true
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
                bottom: tabBar.top
            }
            model: NavMenuModel{
                panelIsParent: false
            }
            interactive: false
            orientation: ListView.Horizontal
            snapMode: ListView.SnapOneItem
            currentIndex: tabBar.currentIndex
            highlightMoveDuration: 275 // seems about right :-D
        }

        Comps.TabBar {
            id: tabBar
            width: parent.width
            anchors.bottom: parent.bottom
            iconNameModel: ["email", "contact-group", "settings", "like"]
            currentIndex: 0
            onCurrentIndexChanged: {
                switch(currentIndex) {
                case 0:
                    menuPage.pageHeader.title = qsTr("Mail")
                    break
                case 1:
                    menuPage.pageHeader.title = qsTr("Contacts")
                    break
                case 2:
                    menuPage.pageHeader.title = qsTr("Settings")
                    break
                case 3:
                    menuPage.pageHeader.title = qsTr("About")
                    break
                }
            }
        }
    }
}


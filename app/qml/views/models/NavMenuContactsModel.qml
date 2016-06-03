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

VisualItemModel {
    ListItem {
        height: abookLayout.implicitHeight
        ListItemLayout {
            id: abookLayout
            title.text: qsTr("Addressbook")

            Icon {
                height: units.gu(3)
                width: height
                name: "address-book-app-symbolic"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
    }
    ListItem {
        height: recentLayout.implicitHeight
        ListItemLayout {
            id: recentLayout
            title.text: qsTr("Recent contacts")

            Icon {
                height: units.gu(3)
                width: height
                name: "contact"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
    }
    ListItem {
        height: importLayout.implicitHeight
        ListItemLayout {
            id: importLayout
            title.text: qsTr("Import contacts")

            Icon {
                height: units.gu(3)
                width: height
                name: "import"
                SlotsLayout.position: SlotsLayout.Leading
            }
            ProgressionSlot{}
        }
    }
}


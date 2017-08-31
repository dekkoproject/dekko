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
import Dekko.Ubuntu.Constants 1.0

PageContent {

    default property alias content: col.data
    property alias spacing: col.spacing
    property int margins: Style.defaultSpacing

    function positionAtBeginning() {
        flicker.contentY = 0
    }
    ScrollView {
        anchors.fill: parent
        Flickable {
            id: flicker

            anchors.fill: parent
            contentHeight: col.height + units.gu(5)
            Column {
                id: col
                anchors {
                    left: parent.left
                    leftMargin: margins
                    top: parent.top
                    topMargin: margins
                    right: parent.right
                    rightMargin: margins
                }
                spacing: Style.smallSpacing
            }
        }
    }
}

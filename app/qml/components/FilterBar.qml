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

Item {
    id: filterBar

    property var filterSections: []
    property alias selectedIndex: row.currentIndex
    height: units.gu(0)
    width: parent.width

    Behavior on height {
        UbuntuNumberAnimation {duration: UbuntuAnimation.SnapDuration}
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#ffffff"
    }

//    Connections {
//        target: dekko.accountsManager
//        onCurrentAccountChanged: {
//            row.currentIndex = 0
//            row.positionViewAtIndex(0, ListView.Visible )
//        }
//    }

    ListView {
        id: row
        anchors {
            fill: parent
            leftMargin: units.gu(2)
//            rightMargin: units.gu(2)
        }
        highlightFollowsCurrentItem: true
        clip: true
        snapMode: ListView.SnapToItem
        orientation: ListView.Horizontal
        layoutDirection: ListView.LeftToRight
        cacheBuffer: units.gu(500)
        currentIndex: 0
        model: filterSections
        delegate: AbstractButton {
            width: label.implicitWidth + units.gu(3.5)
            height: parent.height
            Label {
                id: label
                text: modelData
                color: row.currentIndex === model.index ? UbuntuColors.blue : UbuntuColors.ash
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: units.gu(0.2)
                fontSize: "small"
//                font.weight: Style.common.fontWeight
                renderType: Text.NativeRendering
            }
            Rectangle {
                color: row.currentIndex === model.index ? UbuntuColors.blue : UbuntuColors.lightGrey
                height: units.dp(2)
                width: parent.width
                anchors.bottom: parent.bottom
            }
            onClicked: row.currentIndex = model.index
        }

    }
}

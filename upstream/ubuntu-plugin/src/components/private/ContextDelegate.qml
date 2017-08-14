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
import Dekko.Ubuntu.Components 1.0

ListItem {
    id: root
    height: ctxtLayout.height
    divider.visible: false
    color: UbuntuColors.porcelain
    property string description
    property alias actionIcon: leadingImage.name
    property alias actionIconColor: leadingImage.color
    property alias selectedIcon: trailingImage.name
    property alias selectedIconColor: trailingImage.color
    property alias selected: trailingImage.visible

    // So aparently trying to show a highlight on mouse over is
    // a partcularly annoying thing! Just binding to MouseArea::containsMouse
    // results in all delegates being highlighted on the context menu's completion
    // So instead we have to late bind containsMouse using a short timer binding
    // containsMouse to the highlight rectangle.
    Rectangle {
        id: h
        property bool canShow: false
        anchors.fill: parent
        visible: false
        color: UbuntuColors.silk
    }
    MouseArea {
        id: ma
        anchors.fill: h
        onClicked: root.clicked()
    }
    Binding {
        target: h
        property: "visible"
        value: (ma.containsMouse || ma.pressed)
        when: h.canShow
    }
    Timer {
        id: bindTimer
        interval: 250
        repeat: false
        onTriggered: {
            h.canShow = true
            ma.hoverEnabled = true
        }
    }

    ListItemLayout {
        id: ctxtLayout
        height: units.gu(5)
        title.text: description
        CachedImage {
            id: leadingImage
            height: units.gu(3); width: height
            SlotsLayout.position: SlotsLayout.Leading
        }
        CachedImage {
            id: trailingImage
            height: units.gu(3); width: height
        }
    }



    Component.onCompleted: bindTimer.start()
}

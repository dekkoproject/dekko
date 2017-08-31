import QtQuick 2.4
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
import Ubuntu.Components 1.3

AbstractButton {
    id: button
    
    property string iconName
    property alias iconColor: icon.color
    property bool showText: false
    property int verticalCenterOffset: 0
    property int iconSize: units.gu(2)
    property alias highlightColor: h.color
    
    width: showText ? description.width : units.gu(5)
    height: width
    
    UbuntuShape {
        id: h
        anchors {
            fill: parent
            topMargin: units.gu(1)
            bottomMargin: units.gu(1)
        }
        aspect: UbuntuShape.Flat
        radius: "small"
        color: UbuntuColors.silk
        visible: button.pressed
    }
    
    Icon {
        id: icon
        anchors.centerIn: parent
        anchors.verticalCenterOffset: button.verticalCenterOffset
        width: iconSize
        height: width
        name: action && action.iconName ? action.iconName : ""
        source: action && action.iconSource ? action.iconSource : ""
        opacity: action ? (action.enabled ? 1.0 : 0.5) : 1.0
    }
    Label {
        id: description
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: icon.bottom
        }
        text: action ? action.text : ""
        fontSize: "small"
        visible: showText
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
    }
}

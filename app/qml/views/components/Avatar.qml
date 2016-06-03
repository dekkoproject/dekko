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
import "../utils/UiUtils.js" as UiUtils
import "../../constants"

PixelPerfectItem {
    id: avatar
    property string name
    property string initials
    property string email
    property string radius: "small"
    property bool validContact: true
    property string fontSize: "large"

    height: units.gu(6)
    width: height
    Component {
        id: component_inner_av
        UbuntuShape {
            id: avatarCircle
            anchors.fill: parent
            anchors.centerIn: parent
            aspect: UbuntuShape.Flat
            radius: avatar.radius
            backgroundColor: UiUtils.getIconColor(name)
            visible: validContact
            //name: "avatar-circle"
            Label {
                id: initialsLabel
                color: "#FFFFFF"
                text: validContact ? initials : ""
                anchors.centerIn: parent
                fontSize: avatar.fontSize
            }

            Icon {
                id: contactIcon
                name: "contact"
                anchors.centerIn: parent
                height: parent.height - Style.defaultSpacing
                width: height
                color: "white"
                visible: !initialsLabel.text
            }
        }
    }
    Loader {
        id: loader_inner_av
        anchors.fill: parent
        active: !inner_name.visible
        sourceComponent: component_inner_av
    }
    UbuntuShapeGravatar {
        id: inner_name
        radius: avatar.radius
        emailAddress: validContact /*&& GlobalSettings.preferences.gravatarAllowed */? email : ""
        clip: true
        anchors.fill: parent
        visible: status === Image.Ready
    }
}


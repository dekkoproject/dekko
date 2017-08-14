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
import Ubuntu.Components.Popups 1.3
import Dekko.Ubuntu.Components 1.0

// Should be used on a list model of type MailAddress. i.e QQmlObjectListModel<MailAddress>
UbuntuShape {
    id: delegate

    property bool composeMode: false
    property int type: -1

    aspect: UbuntuShape.Flat
    color: Qt.rgba(0, 0, 0, 0.05)
    radius: "small"
    height: units.gu(3)
    width: inner_avatar.width + label.width + units.gu(1.5)

    Avatar {
        id: inner_avatar
        width: height
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            margins: units.dp(1)
        }
        name: model.qtObject.name
        initials: model.qtObject.initials
        email: model.qtObject.address
        fontSize: "x-small"
        validContact: true
    }

    Label {
        id: label
        anchors {
            left: inner_avatar.right
            leftMargin: units.gu(0.5)
            verticalCenter: parent.verticalCenter
        }
        text: model.qtObject.name
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            PopupUtils.open(Qt.resolvedUrl("../popovers/RecipientPopover.qml"), delegate, {address: model.qtObject, composeMode: composeMode, index: model.index, type: type})
        }
    }
}


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
import Dekko.Components 1.0
import Dekko.Mail.Settings 1.0

PixelPerfectItem {
    id: item
    property string emailAddress
    property alias source: inner_image.source
    property alias status: inner_image.status
    property alias sourceSize: inner_image.sourceSize
    readonly property url circleMask: "circle.png"
    property alias radius: shape.radius

    onEmailAddressChanged: {
        inner_image.source = ""
    }

    UbuntuShape {
        id: shape
        anchors.fill: parent
        aspect: UbuntuShape.Flat
    }

    Image {
        id: inner_image
        anchors.fill: parent
        smooth: true
        visible: false
        asynchronous: true
        ImageHelper on source {
            id: helper
            size: item.width
            gravatarEmail: (size > 0 && PolicyManager.views.gravatarEnabled) ? emailAddress : ""
        }
        onSourceChanged: shape.source = inner_image;
    }
}

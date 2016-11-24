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
import Dekko.Components 1.0

PixelPerfectItem {
    property alias color: helper.color
    property alias name: helper.icon
    // set's the images horizontal & vertical ratio in terms of width/height of the component
    // Default = 1.0
    property alias horizontalRatio: helper.horizontalRatio
    property alias verticalRatio: helper.verticalRatio
    Image {
        id: root
        anchors.fill: parent
        cache: true;
        smooth: false;
        fillMode: Image.Pad;
        asynchronous: true;
        antialiasing: false;
        ImageHelper on source {
            id: helper
            size: root.height
        }
    }
}
